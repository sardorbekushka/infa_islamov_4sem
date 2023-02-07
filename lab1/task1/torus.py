import gmsh
import sys
import numpy as np

gmsh.initialize()

gmsh.model.add("thor")

# радиус тора
r1 = 1
# радиус сечения тора
r2 = 0.5

# ось вращения тора
gmsh.model.occ.addCircle(0, 0, 0, r1, 1)
gmsh.model.occ.addWire([1], 1)

# образующая тора
gmsh.model.occ.addDisk(1, 0, 0, r2, r2, 1)
gmsh.model.occ.rotate([(2, 1)], 0, 0, 0, 1, 0, 0, np.pi / 2)

# каркас тора
gmsh.model.occ.addPipe([(2, 1)], 1)

gmsh.model.occ.synchronize()

gmsh.option.setNumber("Mesh.MeshSizeFromCurvature", 50)

gmsh.model.mesh.generate(3)

gmsh.write("thor.msh")

if '-nopopup' not in sys.argv:
    gmsh.fltk.run()

gmsh.finalize()
