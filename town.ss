
define
meshName ground0.obj
collision 1
tex0 sand.png,1
tex1 ground0Light.png,1
tex2 sand_norm.png,1
tex3 sand.png,1
shader vertex.vs,frag.ps
uniform vcolor,.9,.8,.6,1
uniform tile,20,20,0,0

define
meshName building0.obj
collision 1
tex0 sand.png,1
tex1 building0Light.png,1
tex2 sand_norm.png,1
tex3 sand.png,1
shader vertex.vs,frag.ps
uniform vcolor,.7,.65,.6,1
uniform tile,20,20,0,0


define
meshName sky_box.obj
collision 0
follow 1
castShadow 0
pos 0,0,0
tex0 stars.png,0
shader vertex.vs, skyShader.ps