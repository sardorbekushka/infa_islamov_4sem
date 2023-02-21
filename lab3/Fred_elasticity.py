from fenics import *
from mshr import *
from ufl import nabla_grad
from ufl import nabla_div

# Scaled variables
L = 25
W = 30
mu = 3
rho = 0.1
delta = W / L
gamma = 0.4 * delta**2
beta = 6.25
lambda_ = beta
g = gamma

# Create mesh and define function space
mesh = Mesh('FredTheFrog.xml')
V = VectorFunctionSpace(mesh, 'P', 2)

# Define boundary condition
tol = 1E-14

def clamped_boundary(x, on_boundary):
    return on_boundary and x[2] < 2 + tol

bc = DirichletBC(V, Constant((0, 0, 0)), clamped_boundary)

# Define strain and stress

def epsilon(u):
    return 0.5 * (nabla_grad(u) + nabla_grad(u).T)

def sigma(u):
    return lambda_ * nabla_div(u) * Identity(d) + 2 * mu * epsilon(u)

# Define variational problem
u = TrialFunction(V)
d = u.geometric_dimension()  # space dimension
v = TestFunction(V)
f = Constant((0, 0, -rho * g))
T = Constant((0, 0, 0))
a = inner(sigma(u), epsilon(v)) * dx
L = dot(f, v) * dx + dot(T, v) * ds

# Compute solution
u = Function(V)
solve(a == L, u, bc)

s = sigma(u) - (1./3) * tr(sigma(u)) * Identity(d)  # deviatoric stress
von_Mises = sqrt(3./2 * inner(s, s))
V = FunctionSpace(mesh, 'P', 1)
von_Mises = project(von_Mises, V)

# Compute magnitude of displacement
u_magnitude = sqrt(dot(u, u))
u_magnitude = project(u_magnitude, V)

File('FredTheFrog/displacement.pvd') << u
File('FredTheFrog/von_mises.pvd') << von_Mises
File('FredTheFrog/magnitude.pvd') << u_magnitude
xml_file = XDMFFile('FredTheFrog/FredTheFrog.xdmf')
xml_file.parameters["flush_output"] = True
xml_file.parameters["functions_share_mesh"] = True
xml_file.write(u, 0.0)
xml_file.write(von_Mises, 0.0)