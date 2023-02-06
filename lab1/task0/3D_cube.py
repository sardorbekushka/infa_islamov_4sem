import gmsh
import sys

gmsh.initialize()

gmsh.model.add("cube")

lc = 1e-2

gmsh.model.geo.addPoint(0, 0, 0, lc, 1)
gmsh.model.geo.addPoint(.1, 0, 0, lc, 2)
gmsh.model.geo.addPoint(.1, .1, 0, lc, 3)
gmsh.model.geo.addPoint(0, .1, 0, lc, 4)
gmsh.model.geo.addPoint(0, 0, .1, lc, 5)
gmsh.model.geo.addPoint(.1, 0, .1, lc, 6)
gmsh.model.geo.addPoint(.1, .1, .1, lc, 7)
gmsh.model.geo.addPoint(0, .1, .1, lc, 8)

for i in range(1, 5):
    gmsh.model.geo.addLine(i, i % 4 + 1, i)
    gmsh.model.geo.addLine(i, i + 4, i + 4)
    gmsh.model.geo.addLine(i + 4, i % 4 + 5, i + 8)

# gmsh.model.geo.addCurveLoop([1, 2, 3, 4], 1)
# gmsh.model.geo.addPlaneSurface([1], 1)

for i in range(2):
    j = i * 8
    gmsh.model.geo.addCurveLoop([j + 1, j + 2, j + 3, j + 4], i + 1)
    gmsh.model.geo.addPlaneSurface([i + 1], i + 1)

for i in range(1, 5):
    gmsh.model.geo.addCurveLoop([i, i % 4 + 5, -i - 8, -i - 4], i + 2)
    gmsh.model.geo.addPlaneSurface([i + 2], i + 2)

l = gmsh.model.geo.addSurfaceLoop([i + 1 for i in range(6)])
gmsh.model.geo.addVolume([l])

gmsh.model.geo.synchronize()

gmsh.model.mesh.generate(3)

gmsh.write("cube.msh")
# gmsh.write("cube.geo_unrolled")

if '-nopopup' not in sys.argv:
    gmsh.fltk.run()

gmsh.finalize()
