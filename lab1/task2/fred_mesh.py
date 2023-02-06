import gmsh
import math
import os
import sys

gmsh.initialize()

# Let's merge an STL mesh that we would like to remesh (from the parent
# directory):
path = os.path.dirname(os.path.abspath(__file__))
gmsh.merge(os.path.join(path, 'FredTheFrog.stl'))

# Angle between two triangles above which an edge is considered as sharp:
angle = 40

forceParametrizablePatches = False
includeBoundary = True
curveAngle = 180

gmsh.model.mesh.classifySurfaces(angle * math.pi / 180., includeBoundary,
                                 forceParametrizablePatches,
                                 curveAngle * math.pi / 180.)

# Create a volume from all the surfaces
s = gmsh.model.getEntities(2)
l = gmsh.model.geo.addSurfaceLoop([s[i][1] for i in range(len(s))])
gmsh.model.geo.addVolume([l])

gmsh.model.geo.synchronize()

gmsh.model.mesh.generate(3)
gmsh.write('FredTheFrog.msh')

# Launch the GUI to see the results:
if '-nopopup' not in sys.argv:
    gmsh.fltk.run()

gmsh.finalize()
