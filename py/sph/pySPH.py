#!/usr/bin/env python
# pySPH
# Time-stamp: <2014-09-24 19:59:24 hamada>

import OpenGL 
OpenGL.ERROR_ON_COPY = True 
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
from OpenGL.constants import GLfloat
import sys, time, math, random

vec4 = GLfloat_4
particles = [ ]
help_msg = [
"-------------------------",
"k: rotate simulation box",
"j: rotate simulation box",
"J: box size --",
"K: box size ++",
"t: dt--",
"T: dt++",
"r: shuffle particles",
"e: wall(e) --",
"E: wall(e) ++",
"v: viscosity ++",
"V: viscosity --",
"[Space]: increase particles",
"-: reduce particles",
"1: for debug",
"2: change 3D->2D",
"q: quit",
"h: show this message",
]


class SPH_Parameters: 
    def __init__(self, grav_const = [0., -9.8, 0.], scale=0.004,
                 mass=0.00020543, pressure_coef=1.00,
                 boundary_damp = 256.0, boundary_repu = 10000.0,
                 boundary_eps  = 0.00001, boundary_radius = 0.004,
                 fluid_density = 600.0, effective_radius=0.01, h9 = 0.0, h2 = 0.0, h6 = 0.0,
                 sim_box_min = [-10., -10.0, -10.0], 
                 sim_box_max = [ 10.,  10.0,  10.0],
                 init_dist_min = [ 4.5,  4.5,  4.5],
                 init_dist_max = [ 5.5,  5.5,  5.5],
                 viscosity=0.2, limit=200., dt=0.004):
        self.viscosity = viscosity #  (Pa.s) = 1 kg m^-1 s^-1
        self.limit = limit         #  velocity limitation at boundary condition
        self.dt    = dt            #  delta time for each time-stemps (shared time-step scheme)
        self.grav_const = grav_const
        self.scale = scale         # multiples x,y,z by this value
        self.boundary_damp    = boundary_damp   # constant of damping for boundary rebounding
        self.boundary_repu    = boundary_repu   # constant of repulsion for boundary rebounding
        self.boundary_eps     = boundary_eps    # constant for boundary rebounding
        self.boundary_radius  = boundary_radius # constant for boundary rebounding (meter)
        self.mass = mass # mass for each SPH particles (kg)
        self.fluid_density = fluid_density # all of SPH particles are moving to keep this density (kg / m^3)
        self.effective_radius  = effective_radius 
        self.h2 = h2 # effective_radius ^ 2 : after init, you need to calculate by yourself 
        self.h6 = h6 # effective_radius ^ 6 : after init, you need to calculate by yourself 
        self.h9 = h9 # effective_radius ^ 9 : after init, you need to calculate by yourself 
        self.pressure_coef = pressure_coef # coefficient for pressure
        self.sim_box_min = sim_box_min # simulation box size
        self.sim_box_max = sim_box_max # simulation box size
        self.init_dist_min = init_dist_min # initial distribution size
        self.init_dist_max = init_dist_max # initial distribution size

class Viewer: 
    def __init__(self, 
                 is_3D=True, mouse_l=0, mouse_m=0, mouse_r=0,
                 view_rot=[0., 180., 0.],
                 r_max = 20., v_max=1., radius_max=0.001,
                 fps_calc = 0., fps_phys=0., sim_step=0, sim_time=0.,
                 mpos = [0,0], trans = [0., 1.4, -14.]):
        self.is_3D = is_3D
        self.mouse_l = mouse_l
        self.mouse_m = mouse_m
        self.mouse_r = mouse_r
        self.view_rot = [view_rot[i] for i in range (0, 3)]
        self.r_max = r_max
        self.v_max = v_max
        self.radius_max = radius_max
        self.mpos = [mpos[i] for i in range (0, 2)]
        self.trans = [trans[i] for i in range (0, 3)]
        self.fps_calc = fps_calc
        self.fps_phys = fps_phys
        self.sim_step = sim_step
        self.sim_time = sim_time

    def adj_grav(self, sphsim):
        z = self.view_rot[2]
        gx = sphsim.grav_const[0]
        gy = sphsim.grav_const[1]
        g  = (gx * gx + gy * gy)**0.5
        sphsim.grav_const[0] = -g * math.sin(2.*math.pi* z /360.)
        sphsim.grav_const[1] = -g * math.cos(2.*math.pi* z /360.)
#        print "grav_const: ", sphsim.grav_const
#        print "view_rot: ", self.view_rot


class Particle: 
    def __init__(self, gl_index=0, gl_color=vec4(1.,1.,1.,1.) , mass=0., r=[0., 0., 0.], v=[0., 0., 0.], a=[0., 0., 0.], f=[0., 0., 0.], rho=0, p=0, radii=0.001):
        self.gl_index = gl_index # index for OpenGL display list
        self.gl_color = gl_color
        self.m  = mass
        self.r = [r[i] for i in range (0, 3)]
        self.v = [v[i] for i in range (0, 3)]
        self.a = [a[i] for i in range (0, 3)]
        self.f = [f[i] for i in range (0, 3)]
        self.p = p
        self.radii = radii


viewer = Viewer()
sparams = SPH_Parameters()


def sph_init():
    global particles, sparams

    #---------------------------------------------------------
    sparams.sim_box_min   = [ -6.0, -10.0,  -1.5]
    sparams.sim_box_max   = [  6.0,  10.0,   1.5]
    sparams.init_dist_min = [ -3.5, -0.5, -0.5]
    sparams.init_dist_max = [ -1.5,  5.5,  0.5]
    sparams.scale = 0.01
    sparams.viscosity = 0.1 # pascal-second (Pa.s) = 1 kg m^-1 s^-1
    sparams.dt = 0.004 * 2.0
    sparams.boundary_eps = 4.37893890381e-3

    # initial distance between SPH particles
    d  = ((sparams.mass / sparams.fluid_density)**(1/3.0))  # (meter)
    d = d * 0.87 / sparams.scale # scaled

    xmax = sparams.init_dist_max[0]
    xmin = sparams.init_dist_min[0]
    ymax = sparams.init_dist_max[1]
    ymin = sparams.init_dist_min[1]
    zmax = sparams.init_dist_max[2]
    zmin = sparams.init_dist_min[2]

    z = zmin
    while  z <= zmax:
        y = ymin
        while y <= ymax:
            x = xmin
            while x <= xmax:
                p = Particle()
                # print "%.2f, %.2f, %.2f" % (x,y,z) # jjj
                p.r[0] = x
                p.r[1] = y
                p.r[2] = z
                p.r[0] += -0.05 + random.random()*0.1
                p.r[1] += -0.05 + random.random()*0.1
                p.r[2] += -0.05 + random.random()*0.1
                p.v[0] = p.v[1] = p.v[2] = 0.0
                particles.append(p)
                x += d
            y += d
        z += d
        print "num of particles: ", len(particles)

    # calculate h9
    h = sparams.effective_radius
    h2 = h * h
    h4 = h2 * h2
    h6 = h4 * h2
    h8 = h4 * h4
    h9 = h8 * h
    sparams.h2 = h2
    sparams.h6 = h6
    sparams.h9 = h9


# density(rho) and pressure(p)
#  using 6-th order polynominal Kernel
def calculate_rho_p():
    global particles

    m = sparams.mass # mass for each SPH particles
    h =  sparams.effective_radius
    h2 = sparams.h2
    h9 = sparams.h9

    for pi in particles:
        sum = 0.0
        for pj in particles:
            if pi.gl_index == pj.gl_index: continue
            dr = [ (pi.r[k] - pj.r[k]) * sparams.scale for k in range(len(pi.r)) ]
            r2 =  dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2]
            if h2  > r2 :
                c = h2 - r2
                sum += c * c * c

        pi.rho = sum * m * 315.0 / ( 64.0 * math.pi * h9 )                # density 
        pi.p   = (pi.rho - sparams.fluid_density) * sparams.pressure_coef # pressure
        if pi.rho < 1e-8:  pi.rho = 1e-8
        pi.rho = 1.0 / pi.rho


# calclate force
#   using Spiky Kernel
def calculate_force():
    global particles
    h  = sparams.effective_radius
    h6 = sparams.h6
    h6pi = h6 * math.pi
    vvh = 45.0 / h6pi
    cch = 0.5 * vvh

    for pi in particles:
        f_i=[0.,0.,0.]
        for pj in particles:
            if pi.gl_index == pj.gl_index: continue
            dr = [ (pi.r[k] - pj.r[k]) * sparams.scale for k in range(len(pi.r)) ]
            r  = math.sqrt( dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2] )
            if h > r:
                q = h - r
                cc = cch * q *  ( pi.p + pj.p ) / r 
                vv = sparams.viscosity * vvh
                f_ij = [ cc * dr[k] + vv * (pj.v[k] - pi.v[k]) for k in range(len(pi.v)) ]
                f_ij = [ f_ij[k] * q * pi.rho * pj.rho for k in range(len(f_ij)) ]
                f_i  = [ f_i[k] + f_ij[k] for k in range(len(f_ij)) ]
        pi.f = f_i


def calculate_boundary_condition():
    global particles
    c_scale = sparams.scale
    c_da  = sparams.boundary_damp
    c_re  = sparams.boundary_repu
    c_eps = sparams.boundary_eps
    c_r   = sparams.boundary_radius
    c_m   = sparams.mass
    c_min = sparams.sim_box_min
    c_max = sparams.sim_box_max

    for pi in particles:
        acc = [ pi.f[k] * c_m for k in range(0,3) ]
        acc2 = acc[0]*acc[0] + acc[1]*acc[1] + acc[2]*acc[2]
        if acc2 > sparams.limit**2.: acc = [ acc[k] * sparams.limit / (acc2**0.5) for k in range(0,3) ]
        # wall X
        diff = 2.0 * c_r - ( pi.r[0] - c_min[0] ) * c_scale
        if  diff > c_eps :          acc[0] += c_re * diff - c_da * pi.v[0]
        diff = 2.0 * c_r - ( c_max[0] - pi.r[0] ) * c_scale
        if diff > c_eps:            acc[0] -= c_re * diff + c_da * pi.v[0]
        # wall Y
        diff = 2.0 * c_r - ( pi.r[1] - c_min[1] ) * c_scale
        if diff > c_eps:            acc[1] += c_re * diff - c_da * pi.v[1]
        diff = 2.0 * c_r - ( c_max[1] - pi.r[1] ) * c_scale
        if diff > c_eps:            acc[1] -= c_re * diff + c_da * pi.v[1]
        # wall Z
        diff = 2.0 * c_r - (pi.r[2] - c_min[2]) * c_scale
        if diff > c_eps:            acc[2] += c_re * diff - c_da * pi.v[2]
        diff = 2.0 * c_r - ( c_max[2] - pi.r[2] ) * c_scale
        if diff > c_eps:            acc[2] -= c_re * diff + c_da * pi.v[2]

        # gravitational force from the earth
        acc = [ acc[k] + sparams.grav_const[k] for k in range(0,3) ]
        pi.a = acc



def time_integration():
    time_integration_LeapFrog2ndOrder()

# 2nd order Leapfrog (Velocity Verlet scheme) integration 
is_first_integral=True
def time_integration_LeapFrog2ndOrder():
    global particles
    global is_first_integral

    dt = sparams.dt

    if is_first_integral==True:
        calculate_rho_p()
        calculate_force()
        calculate_boundary_condition()
        is_first_integral=False

    for pi in particles:
        pi.v = [ pi.v[k] + pi.a[k] * dt * 0.5 for k in range(0,3) ]
        pi.r = [ pi.r[k] + pi.v[k] * dt for k in range(0,3) ]
        if viewer.is_3D is not True:  pi.r[2] = 0.0 

    calculate_rho_p()
    calculate_force()
    calculate_boundary_condition()

    for pi in particles:
        pi.v = [ pi.v[k] + pi.a[k] * dt * 0.5 for k in range(0,3) ]

    viewer.sim_time += dt
    viewer.sim_step += 1


# 1st order Euler integration 
def time_integration_Euler1stOrder():
    global particles

    dt = sparams.dt

    calculate_rho_p()
    calculate_force()
    calculate_boundary_condition()

    for pi in particles:
        pi.v = [ pi.v[k] + pi.a[k] * dt for k in range(0,3) ]
        pi.r = [ pi.r[k] + pi.v[k] * dt for k in range(0,3) ]
        if viewer.is_3D is not True:  pi.r[2] = 0.0 

    viewer.sim_time += dt
    viewer.sim_step += 1



def simulate():
    global particles
    time_integration()

def  _glutSolidSphere(radius):
        glutSolidSphere(radius*300., 32, 32)

def add_particle():
    global particles
    r_max = viewer.r_max
    v_max = viewer.v_max
    p = Particle()
    p.r[0] = (0.5 - random.random()) * r_max / 8.
    p.r[1] = 4.95 #(0.5 - random.random()) * r_max / 8.
    p.r[2] = (0.5 - random.random()) * r_max / 8.
    for k in range(0,3):  p.v[k] = (0.5 - random.random()) * v_max 
    p.a[0] = p.a[1] = p.a[2] = 0.
    p.gl_color  = vec4(random.random(), random.random(), random.random(), 0.0)
    p.gl_index = glGenLists(1)
    glNewList(p.gl_index, GL_COMPILE)
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p.gl_color)
    _glutSolidSphere(p.radii)
    glEndList()
    particles.append(p)


def del_particle():
    global particles    
    particles.pop()


def reset_pos_vel_acc():
    global particles
    for i in range(0, len(particles)):
        p = particles[i]
        p.r[0] = (0.5 - random.random()) * viewer.r_max
        p.r[1] = (0.5 - random.random()) * viewer.r_max
        p.r[2] = (0.5 - random.random()) * viewer.r_max
        p.v[0] = (0.5 - random.random()) * sparams.limit/4.
        p.v[1]= (0.5 - random.random()) * sparams.limit/4.
        p.v[2] = (0.5 - random.random()) * sparams.limit/4.
        particles[i] = p

tStart = t0 = time.time()
frames = 0

def framerate():
    global t0, frames, viewer
    t = time.time()
    frames += 1
    if t - t0 >= 1.0:
        seconds = t - t0
        fps_calc = frames/seconds
        fps_phys =  viewer.sim_time/seconds
#        print "%.0f frames in %3.1f seconds = %6.3f FPS" % (frames,seconds,fps_calc)
        t0 = t
        frames = 0
        viewer.fps_calc = fps_calc
        viewer.fps_phys = fps_phys


def draw_text():
    text_list = [ ]
    text_list.append( "simulation time : %f" % viewer.sim_time )
    text_list.append( "simulation steps: %d" % viewer.sim_step )
    text_list.append( "FPS:       %f" % viewer.fps_calc )
    text_list.append( "FPS(Phys): %f" % viewer.fps_phys )
    text_list.append( "# of particles: %d" % len(particles) )
    text_list.append( "viscosity: %.1e" % sparams.viscosity )
    text_list.append( "wall(e): %.2f" % (9.85e-3 / sparams.boundary_eps ) )
    text_list.append( "dt: %.2e" % sparams.dt)

    glColor4f( 1.0, 1.0, 0.5, 1.0 )
    glDisable(GL_DEPTH_TEST)
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    glLoadIdentity()
    glTranslatef(-.9, .8, 0)
    glScalef(.0006, .0006, 1)

    y = 10

    for s in text_list:
        glRasterPos2f(2.0, y)
        y -= 45.
        for c in s:
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(c))

    glPopMatrix()
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)
    glEnable(GL_DEPTH_TEST)

def draw_box():
    global r_max
    c_min = sparams.sim_box_min
    c_max = sparams.sim_box_max

    glLineWidth(2)
    glBegin(GL_LINES)
    glColor3f(1.0, 1.0, 1.0)
    glVertex3f(c_min[0], c_min[1], c_min[2]),    glVertex3f(c_max[0], c_min[1], c_min[2])
    glVertex3f(c_min[0], c_max[1], c_min[2]),    glVertex3f(c_max[0], c_max[1], c_min[2])
    glVertex3f(c_min[0], c_min[1], c_max[2]),    glVertex3f(c_max[0], c_min[1], c_max[2])
    glVertex3f(c_min[0], c_max[1], c_max[2]),    glVertex3f(c_max[0], c_max[1], c_max[2])

    glVertex3f(c_min[0], c_min[1], c_min[2]),    glVertex3f(c_min[0], c_max[1], c_min[2])
    glVertex3f(c_max[0], c_min[1], c_min[2]),    glVertex3f(c_max[0], c_max[1], c_min[2])
    glVertex3f(c_min[0], c_min[1], c_max[2]),    glVertex3f(c_min[0], c_max[1], c_max[2])
    glVertex3f(c_max[0], c_min[1], c_max[2]),    glVertex3f(c_max[0], c_max[1], c_max[2])

    glVertex3f(c_min[0], c_min[1], c_min[2]),    glVertex3f(c_min[0], c_min[1], c_max[2])
    glVertex3f(c_min[0], c_max[1], c_min[2]),    glVertex3f(c_min[0], c_max[1], c_max[2])
    glVertex3f(c_max[0], c_min[1], c_min[2]),    glVertex3f(c_max[0], c_min[1], c_max[2])
    glVertex3f(c_max[0], c_max[1], c_min[2]),    glVertex3f(c_max[0], c_max[1], c_max[2])
    glEnd()


def draw():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    gluLookAt(0., 0.,-40.+viewer.trans[2], 0.0, 0.0,  0.0, 0.0, 1.0, 0.0) 

    glPushMatrix()
    glRotatef(viewer.view_rot[0], 1.0, 0.0, 0.0)
    glRotatef(viewer.view_rot[1], 0.0, 1.0, 0.0)
    glRotatef(viewer.view_rot[2], 0.0, 0.0, 1.0)

    for i in range(0, len(particles)):
        p = particles[i]
        glPushMatrix()
        glTranslatef(p.r[0], p.r[1], p.r[2])
        glCallList(p.gl_index)
        glPopMatrix()

    draw_box()
    glPopMatrix()

    draw_text()
    glutSwapBuffers()

    framerate()
    simulate()
    
def idle():
    glutPostRedisplay()
    
    
def adj_grav():
    global viewer
    global sparams
    viewer.adj_grav(sparams)

# change view angle, exit upon ESC
def key(k, x, y):
    global viewer, sparams

    if k == 'k':
        viewer.view_rot[2] += 5.0
    elif k == 't':  # cahnge the box size
        sparams.dt *= 0.8
        print sparams.dt
    elif k == 'T':  # cahnge the box size
        sparams.dt += 0.0001
        print sparams.dt
    elif k == 'j':
        viewer.view_rot[2] -= 5.0
    elif k == 'J':  # cahnge the box size
        sparams.sim_box_min   = [ sparams.sim_box_min[k] / 1.3 for k in  range(3)]
        sparams.sim_box_max   = [ sparams.sim_box_max[k] / 1.3 for k in  range(3)]
    elif k == 'K':  # cahnge the box size
        sparams.sim_box_min   = [ sparams.sim_box_min[k] * 1.3 for k in  range(3)]
        sparams.sim_box_max   = [ sparams.sim_box_max[k] * 1.3 for k in  range(3)]
    elif k == 'r':
        reset_pos_vel_acc()
    elif k == 'e':
        sparams.boundary_eps *= 1.5
    elif k == 'E':
        sparams.boundary_eps /= 1.5
        print sparams.boundary_eps
    elif k == ' ':
        add_particle()
    elif k == '-':
        del_particle()
    elif k == '1':
        print particles[0].r, particles[0].v, particles[0].f
    elif k == 'h':
        for s in help_msg: print s 
    elif k == '2':
        if viewer.is_3D:  
            print "3D -> 2D"
            viewer.is_3D = False
        else:
            print "2D -> 3D"
            viewer.is_3D = True
    elif k == 'v':
        sparams.viscosity += 0.005
        print "sparams.viscosity: ", sparams.viscosity
    elif k == 'V':
        sparams.viscosity -= 0.005
        if sparams.viscosity < 0. : sparams.viscosity = 1e-4
        print "sparams.viscosity: ", sparams.viscosity
    elif k == 'q':
        sys.exit(0)
    elif ord(k) == 27: # Escape
        sys.exit(0)
    else:
        return

    adj_grav()
    glutPostRedisplay()


# change view angle
def special(k, x, y):
    global viewer
    
    if k == GLUT_KEY_UP:
        viewer.view_rot[0] += 5.0
    elif k == GLUT_KEY_DOWN:
        viewer.view_rot[0] -= 5.0
    elif k == GLUT_KEY_LEFT:
        viewer.view_rot[1] += 5.0
    elif k == GLUT_KEY_RIGHT:
        viewer.view_rot[1] -= 5.0
    else:
        return
    adj_grav()
    glutPostRedisplay()

def mouse(button, state, x, y):
    global mpos, viewer
    
    if button == GLUT_LEFT_BUTTON:
        if state == GLUT_DOWN: 
            viewer.mpos[0] = x
            viewer.mpos[1] = y
            viewer.mouse_l = 1
        if state == GLUT_UP:
            viewer.mouse_l = 0
    elif button == GLUT_MIDDLE_BUTTON:
        if state == GLUT_DOWN:
            viewer.mpos[0] = x
            viewer.mpos[1] = y
            viewer.mouse_m = 1
        if state == GLUT_UP:
            viewer.mouse_m = 0
    elif button == GLUT_RIGHT_BUTTON:
        if state == GLUT_DOWN:
            viewer.mpos[0] = x
            viewer.mpos[1] = y
            viewer.mouse_r = 1
        if state == GLUT_UP:
            viewer.mouse_r = 0
    else:
        return

def motion(x, y):
    global viewer
    print "motion: ", x, y, viewer.trans
    if viewer.mouse_l == 1 :
        speed = 1.2
        viewer.view_rot[0] = (y - viewer.mpos[1]) * speed
        viewer.view_rot[2] = (x - viewer.mpos[0]) * speed
    elif viewer.mouse_r == 1:
        viewer.trans[1] += (x - viewer.mpos[0]) * 0.01
        viewer.trans[2] -= (y - viewer.mpos[1]) * 0.01
        print viewer.trans

    if viewer.mouse_l ==1 or viewer.mouse_m == 1 or viewer.mouse_r == 1:
        glutPostRedisplay()
    adj_grav()

# new window size or exposure
def reshape(width, height):
    h = float(height) / float(width)
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
#    glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0)
    gluPerspective(30.0, h, 0.001, 1000.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    glTranslatef(0.0, 0.0, -40.0)

def init():
    global particles
    global r_max

    random.seed(3141592653589793238462643383279502884)
    pos   = vec4(-5.0, -5.0, 10.0, 0.0)
    red   = vec4(0.8, 0.1, 0.0, 1.0)
    green = vec4(0.0, 0.8, 0.2, 1.0)
    blue  = vec4(0.2, 0.2, 1.0, 1.0)

#    glLightfv(GL_LIGHT0, GL_SPECULAR, vec4(0.5, 1.0,  0.0, 0.5))
    glLightfv(GL_LIGHT0, GL_AMBIENT, vec4(0.5, 0.1,  -0.1, 0.1))
    glLightfv(GL_LIGHT1, GL_AMBIENT, vec4(0.5, 0.1,  1.0,  0.1))
    glEnable(GL_CULL_FACE)
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glEnable(GL_LIGHT1)
    glEnable(GL_DEPTH_TEST)

    sph_init()

    for i in range(0, len(particles)):
        p = particles[i]
        p.gl_color  = vec4(random.random(), random.random(), random.random(), 0.0)
        p.gl_index = glGenLists(1)
        glNewList(p.gl_index, GL_COMPILE)
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p.gl_color)
        _glutSolidSphere(p.radii)
        glEndList()

    glEnable(GL_NORMALIZE)


def visible(vis):
    if vis == GLUT_VISIBLE:
        glutIdleFunc(idle)
    else:
        glutIdleFunc(None)


if __name__ == '__main__':
    glutInit(sys.argv)
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH)

    glutInitWindowPosition(800, 0)
    glutInitWindowSize(800, 800)
    glutCreateWindow("pySPH")
    init()
    
    glutDisplayFunc(draw)
    glutReshapeFunc(reshape)
    glutKeyboardFunc(key)
    glutSpecialFunc(special)
    glutMouseFunc(mouse)
    glutMotionFunc(motion)
    glutVisibilityFunc(visible)

    if "-info" in sys.argv:
        print "GL_RENDERER   = ", glGetString(GL_RENDERER)
        print "GL_VERSION    = ", glGetString(GL_VERSION)
        print "GL_VENDOR     = ", glGetString(GL_VENDOR)
        print "GL_EXTENSIONS = ", glGetString(GL_EXTENSIONS)

    glutMainLoop()
    
