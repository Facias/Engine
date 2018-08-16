


define
meshName island_base.obj
collision 1
tex0 white.png,0
tex1 island_baseLight.png,1
tex2 ssss.png,1
shader vertex.vs,frag.ps
uniform vcolor,1,.85,.58,1
uniform tile,20,20,0,0

define
meshName breaker.obj
collision 1
tex0 white.png,0
tex1 island_baseLight.png,1
tex2 pebbles_normalmap.png,1
shader vertex.vs,frag.ps
uniform vcolor,1,.85,.58,1
uniform tile,20,20,0,0

define
meshName teardrop.obj
collision 1
tex0 white.png,0
tex1 island_baseLight.png,1
tex2 pebbles_normalmap.png,1
shader vertex.vs,frag.ps
uniform vcolor,1,.85,.58,1
uniform tile,10,10,0,0

define
meshName branches.obj
collision 1
culling 0
tex0 leaves0.png
pos 0,25,10
shader fVertex.vs,foliageFrag.ps
uniform vcolor,1,.6,.1,1
uniform tile,1,1,0,0

define
meshName grassbush.obj
collision 1
culling 0
pos 0,10,0
tex0 white.png
shader fVertex.vs,foliageFrag.ps
uniform vcolor,.1,.7,.8,1
uniform tile,1,1,0,0

define
meshName branch.obj
collision 1
culling 0
pos 10,10,0
tex0 leaves0.png
shader fVertex.vs,foliageFrag.ps
uniform vcolor,.1,.7,.1,1
uniform tile,1,1,0,0

define
meshPlane 50,50
collision 0
transparent 1
reflectable 0
castShadow 0
tex0 sun.DDS
lookatcam 1
follow 1
shader vertex.vs, lightFrag.ps
uniform tile,1,1,0,0
uniform vcolor,1,1,1,1
pos 200,200,200


define
meshName sky_box.obj
collision 0
follow 1
castShadow 0
pos 0,0,0
scale 1,1,1
shader vertex.vs, skyShader.ps

define
meshName water.obj
pos 10,0,0
scale 10,1,10
tex0 white.png,1
tex1 waterNorm1.png
shader vertex.vs, waterFrag.ps
depth 1
reflect 1
opaque 1
uniform vcolor,1,1,1,1
uniform clarity,16,0,0,0
castShadow 0
water 1
culling 0


define
meshName tea.obj
collision 1
tex0 checker.png
tex1 blank.png
pos 0,30,-20
shader vertex.vs,frag.ps
uniform vcolor,1,1,1,1
uniform tile,20,20,0,0

define
meshName box.obj
collision 1
tex0 checker.png
tex1 blank.png

pos 0,30,-20
shader vertex.vs,frag.ps
uniform vcolor,1,.1,1,1
uniform tile,10,10,0,0
