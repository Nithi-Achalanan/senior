
# coding: utf-8
""" demo on static solving using JAC (experimental) """
# Copyright (c) Benyuan Liu. All Rights Reserved.
# Distributed under the (new) BSD License. See LICENSE.txt for more info.
from __future__ import absolute_import, division, print_function
import matplotlib.pyplot as plt
import numpy as np
import pyeit.eit.jac as jac
import pyeit.eit as eit
import pyeit.eit.protocol as protocol
from pyeit.eit.fem import EITForward
from pyeit.mesh import create, set_perm
from pyeit.mesh.wrapper import PyEITAnomaly_Circle
# Mesh shape is specified with fd parameter in the instantiation, e.g:
# from pyeit.mesh.shape import thorax
# mesh_obj, el_pos = create(n_el, h0=0.05, fd=thorax)  # Default : fd=circle
n_el = 64  # test fem_vectorize
mesh_obj = create(n_el, h0=0.05)
# set anomaly (altering the permittivity in the mesh)
anomaly = [
    PyEITAnomaly_Circle(center=[0.4, 0.4], r=0.2, perm=10.0),
    PyEITAnomaly_Circle(center=[-0.4, -0.4], r=0.2, perm=0.1),
]
# background changed to values other than 1.0 requires more iterations
mesh_new = set_perm(mesh_obj, anomaly=anomaly, background=2.0)
# extract node, element, perm
xx, yy = mesh_obj.node[:, 0], mesh_obj.node[:, 1]
tri = mesh_obj.element
perm = mesh_new.perm
# Calculate then show the results

def compute_first():
    ds = eit.gn(v1, lamb_decay=0.1, lamb_min=1e-5, maxiter=20, verbose=True)
    im = ax.tripcolor(xx, yy, tri, np.real(ds), alpha=1.0, cmap="viridis")
    fig.colorbar(im)

compute_first()
