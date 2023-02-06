import gmsh
import sys

gmsh.initialize()

gmsh.model.add("cylinder")

cylinder = gmsh.model.occ.addCylinder(0, 0, 0, 0, 0, 1, 1)

gmsh.model.occ.synchronize()

gmsh.model.mesh.generate(3)
# gmsh.model.mesh.generate(2)

gmsh.write("cylinder.py.msh")

if '-nopopup' not in sys.argv:
    gmsh.fltk.run()


gmsh.finalize()
