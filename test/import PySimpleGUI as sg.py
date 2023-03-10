from __future__ import absolute_import, division, print_function

import PySimpleGUI as sg
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.backends.backend_tkagg import FigureCanvasAgg
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import io
import PIL
import base64

import matplotlib.pyplot as plt
import numpy as np
import pyeit.eit.bp as bp
import pyeit.eit.protocol as protocol
import pyeit.mesh as mesh
from pyeit.eit.fem import EITForward
from pyeit.mesh.shape import thorax
from pyeit.mesh.wrapper import PyEITAnomaly_Circle

import time
import serial


ser = serial.Serial(
    port='COM4',
    baudrate=9600,
)

if ser.isOpen():
    ser.close()
ser.open()
ser.isOpen()

n = 2
def figure_to_image(figure):
    """
    Draws the previously created "figure" in the supplied Image Element
    :param element: an Image Element
    :param figure: a Matplotlib figure
    :return: The figure canvas
    """

    plt.close('all')        # erases previously drawn plots
    canv = FigureCanvasAgg(figure)
    buf = io.BytesIO()
    canv.print_figure(buf, format='png')
    if buf is None:
        return None
    buf.seek(0)
    return buf.read()
def convert_to_bytes(file_or_bytes, resize=None):
    '''
    Will convert into bytes and optionally resize an image that is a file or a base64 bytes object.
    Turns into  PNG format in the process so that can be displayed by tkinter
    :param file_or_bytes: either a string filename or a bytes base64 image object
    :type file_or_bytes:  (Union[str, bytes])
    :param resize:  optional new size
    :type resize: (Tuple[int, int] or None)
    :return: (bytes) a byte-string object
    :rtype: (bytes)
    '''
    if isinstance(file_or_bytes, str):
        img = PIL.Image.open(file_or_bytes)
    else:
        try:
            img = PIL.Image.open(io.BytesIO(base64.b64decode(file_or_bytes)))
        except Exception as e:
            dataBytesIO = io.BytesIO(file_or_bytes)
            img = PIL.Image.open(dataBytesIO)

    cur_width, cur_height = img.size
    if resize:
        new_width, new_height = resize
        scale = min(new_height/cur_height, new_width/cur_width)
        img = img.resize((int(cur_width*scale), int(cur_height*scale)), PIL.Image.ANTIALIAS)
    with io.BytesIO() as bio:
        img.save(bio, format="PNG")
        del img
        return bio.getvalue()



""" 0. build mesh """
n_el = 16  # nb of electrodes
use_customize_shape = False
if use_customize_shape:
    # Mesh shape is specified with fd parameter in the instantiation, e.g : fd=thorax
    mesh_obj = mesh.create(n_el, h0=0.05, fd=thorax)
else:
    mesh_obj = mesh.create(n_el, h0=0.05)

""" 1. problem setup """
anomaly = PyEITAnomaly_Circle(center=[0.5, 0.5], r=0.1, perm=20.0)

anomaly2 = [PyEITAnomaly_Circle(center=[0.4, 0], r=0.1, perm=20.0),
    PyEITAnomaly_Circle(center=[-0.4, 0], r=0.1, perm=20.0),
    PyEITAnomaly_Circle(center=[0, 0.5], r=0.1, perm=20),
    PyEITAnomaly_Circle(center=[0, -0.5], r=0.1, perm=20), ]
mesh_new = mesh.set_perm(mesh_obj, anomaly=anomaly, background=1.0)
mesh_new2 = mesh.set_perm(mesh_obj, anomaly=anomaly2, background=1.0)

""" 2. FEM forward simulations """
# setup EIT scan conditions
# adjacent stimulation (dist_exc=1), adjacent measures (step_meas=1)
protocol_obj = protocol.create(n_el, dist_exc=1, step_meas=1, parser_meas="std")

# calculate simulated data
fwd = EITForward(mesh_obj, protocol_obj)
v0 = fwd.solve_eit() #initial voltage
v1 = fwd.solve_eit(perm=mesh_new.perm)  #this is real object
v2 = fwd.solve_eit(perm=mesh_new2.perm)

""" 3. naive inverse solver using back-projection """
eit = bp.BP(mesh_obj, protocol_obj)
eit.setup(weight="none")
# the normalize for BP when dist_exc>4 should always be True
ds = 192.0 * eit.solve(v1, v0, normalize=True)

# extract node, element, alpha
pts = mesh_obj.node
tri = mesh_obj.element

fig = plt.figure()
im = plt.tripcolor(pts[:, 0], pts[:, 1], tri, ds)
fig.suptitle('DEMO title    ', fontsize=20)
fig.colorbar(im)
plt.show()

page1 = [
    [
        sg.Text("EIT reconstruct",text_color="black", font=("Cooper", 35),background_color= "white")
        #sg.Text("EIT reconstruct",size=(20,1),text_color="black", font=("Cooper", 35),background_color= "white" , justification='center')
    ],
    [
        sg.Button("Start",key='Action1')
    ]
]

layout1 = [
    [sg.VPush()],
    [sg.Push(), sg.Column(page1,element_justification='c',background_color= "white"), sg.Push()],
    [sg.VPush()]
]

page2_comma = [
    [
        sg.Push(),
        sg.Button("check",key='action_check',size = (6*n,2*n) ,font=("Cooper", 15)),
        sg.Button("static",key='action_static',size = (6*n,2*n) ,font=("Cooper", 15)),
        sg.Button("burst",key='action_burst',size = (6*n,2*n), font=("Cooper", 15)),
        sg.Push()
    ],
]
page2_left = [
    [
        sg.Column(page2_comma,element_justification='c', justification='center') #, justification='center'
    ],
    [
        sg.Push(),sg.Multiline(size=(40,15), font=("Tahoma", 8*n), key='log_box', autoscroll=True),sg.Push()
    ],
]


col1=[[sg.Text('Column1', background_color='red')]]

image_col = sg.Col([[sg.Image(k=(i,j)) for i in range(1)]for j in range(1)],size=(500*n,500*n))

layout2 = [
    [
        sg.Col(page2_left),
        sg.VSeperator(),
        image_col
        
    ]
]



window1 = sg.Window("EIT reconstruct", layout1,size = (900*n,450*n))
window2 = sg.Window("EIT reconstruct", layout2,size = (900*n,450*n))




figure_agg = None

window1_active = window2_active = window3_active = False

i = 0
while True:
    window1_active = True
    event, values = window1.read()
    if event == sg.WIN_CLOSED :
        window1.close()
        break
    if event == "Action1" :
        window1.hide()
        if window2_active == True :
            window2.UnHide()
        else :
            event2, values2 = window2.read()
            window2_active = True
        while True:
            event2, values2 = window2.read()      
            if event2 == "action_static" :
                ser.write('1'.encode())
                '''window2.hide()
                window1.UnHide()
                break'''
            if event2 == "action_check":
                image = figure_to_image(fig)
                image = convert_to_bytes(image, (540*n, 540*n))
                window2[(0,0)].update(data=image)
            if event2 == "action_burst" :
                i+=1
                window2['log_box'].update(str(i)+'\n', append=True)
            if event2 == sg.WIN_CLOSED :
                window2.close()
                window1.close()
                window2_active = False
                break
