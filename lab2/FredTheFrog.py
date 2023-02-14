import vtk
import numpy as np
import gmsh
import math
import os


# Класс расчётной сетки
class CalcMesh:
    def __init__(self, nodes_coords, tetrs_points):
        self.nodes = np.array([nodes_coords[0::3],nodes_coords[1::3],nodes_coords[2::3]])
        self.dist = np.power(self.nodes[0], 2) + np.power(self.nodes[1], 2)
        self.velocity = np.zeros(shape=(3, int(len(nodes_coords) / 3)), dtype=np.double)
        self.tetrs = np.array([tetrs_points[0::4],tetrs_points[1::4],tetrs_points[2::4],tetrs_points[3::4]])
        self.tetrs -= 1
        self.time = 0

    # Метод отвечает за выполнение для всей сетки шага по времени величиной tau
    def move(self, tau):
        self.setVelocity()
        self.nodes += self.velocity * tau
        self.time += tau

    # Метод отвечает за запись текущего состояния сетки в снапшот в формате VTK
    def snapshot(self, snap_number):
        unstructuredGrid = vtk.vtkUnstructuredGrid()
        points = vtk.vtkPoints()

        dist = vtk.vtkDoubleArray()
        dist.SetName("dist")

        vel = vtk.vtkDoubleArray()
        vel.SetNumberOfComponents(3)
        vel.SetName("vel")

        for i in range(0, len(self.nodes[0])):
            points.InsertNextPoint(self.nodes[0,i], self.nodes[1,i], self.nodes[2,i])
            dist.InsertNextValue(self.dist[i])
            vel.InsertNextTuple((self.velocity[0,i], self.velocity[1,i], self.velocity[2,i]))

        unstructuredGrid.SetPoints(points)

        unstructuredGrid.GetPointData().AddArray(dist)
        unstructuredGrid.GetPointData().AddArray(vel)

        for i in range(0, len(self.tetrs[0])):
            tetr = vtk.vtkTetra()
            for j in range(0, 4):
                tetr.GetPointIds().SetId(j, self.tetrs[j,i])
            unstructuredGrid.InsertNextCell(tetr.GetCellType(), tetr.GetPointIds())

        writer = vtk.vtkXMLUnstructuredGridWriter()
        writer.SetInputDataObject(unstructuredGrid)
        writer.SetFileName("FredTheFrogEat/FredTheFrog-step-" + str(snap_number) + ".vtu")
        writer.Write()

    def setVelocity(self):
        pass


class RotateMesh(CalcMesh):
    def __init__(self, nodes_coords, tetrs_points):
        super().__init__(nodes_coords, tetrs_points)
        self.omega = 15
        self.velocity[2] = self.nodes[2]

    def setVelocity(self):
        self.velocity[0] = -self.omega * self.nodes[1]
        self.velocity[1] = self.omega * self.nodes[0]


class EatMesh(CalcMesh):
    def __init__(self, nodes_coords, tetrs_points):
        super().__init__(nodes_coords, tetrs_points)
        # self.maxz = max(self.nodes[2]) * 0.9
        self.maxz = 20
        self.maxVelocity = 100
        self.omega = 50

    def setVelocity(self):
        self.velocity = self.maxVelocity * np.sin(self.omega * self.time - (self.maxz - self.nodes[2]) / 10) * self.nodes / np.sqrt(self.dist)
        self.velocity[2] *= 0
        # self.velocity[1] = self.velocity[0] / self.nodes[0] * self.nodes[1]


class ExplodeMesh(CalcMesh):
    def __init__(self, nodes_coords, tetrs_points):
        super().__init__(nodes_coords, tetrs_points)
        # self.maxz = max(self.nodes[2]) * 0.9
        self.dist2 = self.dist + np.power(self.nodes[2], 2)
        self.vel = 1000

    def setVelocity(self):
        self.velocity = self.vel * self.nodes / np.sqrt(self.dist2)


def getMesh():
    gmsh.initialize()

    try:
        path = os.path.dirname(os.path.abspath(__file__))
        gmsh.merge(os.path.join(path, 'FredTheFrog.stl'))
    except:
        print("Could not load STL mesh: bye!")
        gmsh.finalize()
        exit(-1)

    angle = 40
    forceParametrizablePatches = False
    includeBoundary = True
    curveAngle = 180
    gmsh.model.mesh.classifySurfaces(angle * math.pi / 180., includeBoundary, forceParametrizablePatches,
                                     curveAngle * math.pi / 180.)
    # gmsh.model.mesh.createGeometry()

    s = gmsh.model.getEntities(2)
    l = gmsh.model.geo.addSurfaceLoop([s[i][1] for i in range(len(s))])
    gmsh.model.geo.addVolume([l])

    gmsh.model.geo.synchronize()

    f = gmsh.model.mesh.field.add("MathEval")
    gmsh.model.mesh.field.setString(f, "F", "4")
    gmsh.model.mesh.field.setAsBackgroundMesh(f)

    gmsh.model.mesh.generate(3)

    nodeTags, nodesCoord, parametricCoord = gmsh.model.mesh.getNodes()

    GMSH_TETR_CODE = 4
    tetrsNodesTags = None
    elementTypes, elementTags, elementNodeTags = gmsh.model.mesh.getElements()
    for i in range(0, len(elementTypes)):
        if elementTypes[i] != GMSH_TETR_CODE:
            continue
        tetrsNodesTags = elementNodeTags[i]

    if tetrsNodesTags is None:
        print("Can not find tetra data. Exiting.")
        gmsh.finalize()
        exit(-2)

    print("The model has %d nodes and %d tetrs" % (len(nodeTags), len(tetrsNodesTags) / 4))

    # На всякий случай проверим, что номера узлов идут подряд и без пробелов
    for i in range(0, len(nodeTags)):
        assert (i == nodeTags[i] - 1)
    assert (len(tetrsNodesTags) % 4 == 0)
    gmsh.finalize()
    return nodesCoord, tetrsNodesTags


nodesCoord, tetrsNodesTags = getMesh()

mesh = EatMesh(nodesCoord, tetrsNodesTags)
mesh.snapshot(0)
tau = 0.01

for i in range(1, 18):
    mesh.move(tau)
    mesh.snapshot(i)
