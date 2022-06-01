#!BPY

"""
Name: 'Trezkers model format (.tmf)...'
Blender: 244
Group: 'Export'
Tooltip: 'TMF exporter'
"""
import struct
import bpy
import bmesh

def triangulateObject(obj):
    me = obj.data
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.triangulate(bm, faces=bm.faces[:])
    bm.to_mesh(me)
    bm.free()

def write(context, filename):
    #start = bpy.sys.time()
    if not filename.lower().endswith('.tmf'):
        filename += '.tmf'
    
    ob = context.selected_objects[0]
    triangulateObject(ob)
    uv_layer = ob.data.uv_layers.active.data

    vertices = []

    vertBuff = []
    normBuff = []
    uvBuff   = []
    faceBuff = []

    smooth = True
#    for v in ob.data.vertices:
#        vertBuff.append(v.co)
#    if ob.data.polygons[0].use_smooth:
#        smooth = True
#    for v in ob.data.vertices:
#        normBuff.append(v.normal)

    for poly in ob.data.polygons:
#        if not smooth:
#            normBuff.append(poly.normal)
        for index in poly.loop_indices:
            thisUV = uv_layer[index].uv
            vert = ob.data.loops[index].vertex_index
            found = False
            i = 0
            for v in vertices:
                if v["v"] == vert and v["uv"] == thisUV:
                    found = True;
                    vert = i;
                    break;
                i = i+1
            
            if not found:
                vertices.append({"v":vert, "uv":thisUV})
                vertBuff.append(ob.data.vertices[vert].co)
                normBuff.append(ob.data.vertices[vert].normal)
                uvBuff.append(thisUV)
                vert = len(vertBuff)-1
            faceBuff.append(vert) #index
    print(vertBuff)
    print(normBuff)
    print(uvBuff)
    print(faceBuff)


    #write to file
#    with open(filename, 'w') as ofile:
#        ofile.write("%d " % len(vertBuff)) #num unique vertex
#        ofile.write("%d " % len(faceBuff)) #num indices/uv pairs
#        ofile.write("%d\n" % smooth) #if 1 normals num vertices. if 0 normals num indices
#        for v in vertBuff:
#            ofile.write("%f %f %f\n" % v[:])
#        for v in normBuff:
#            ofile.write("%f %f %f\n" % v[:])
#        for t in uvBuff:
#            ofile.write("%f %f\n" % t[:])
#        for p in faceBuff:
#            ofile.write("%d " % p)
#        ofile.close()
#    return {'FINISHED'}

    #binary file
    with open(filename, 'wb') as ofile:
        ofile.write(struct.pack('H', len(vertBuff))) 
        ofile.write(struct.pack('H', len(faceBuff)))
        ofile.write(struct.pack('H', smooth))
    
        for v in vertBuff:
            ofile.write(struct.pack('3f', v.x, v.y, v.z)) #v[:])) #"%f %f %f " % v[:])
        for v in normBuff:
            ofile.write(struct.pack('3f', v.x, v.y, v.z)) #v[:])) #"%f %f %f " % v[:])
        for t in uvBuff:
            ofile.write(struct.pack('2f', t.x, t.y)) #t[:])) #"%f %f " % t[:])
        for p in faceBuff:
            ofile.write(struct.pack('H', p)) #"%d " % p)
        ofile.close()
    return {'FINISHED'}

    #end = bpy.sys.time()
    #message = 'Successfully exported "%s" in %.4f seconds' % ( Blender.sys.basename(filename), end-start)
    #message = 'Successfully exported "%s"' % ( bpy.sys.basename(filename))
    #print(message)
    return {'FINISHED'}


from bpy_extras.io_utils import ExportHelper
from bpy.types import Operator

class ExportSomeData(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_test.some_data"
    bl_label = "Export Some Data"

    # ExportHelper mixin class uses this
    filename_ext = ".tmf"
    filter_glob: bpy.props.StringProperty(default="*.tmf", options={'HIDDEN'}, maxlen=255)

    def execute(self, context):
        return write(context, self.filepath)

def register():
    bpy.utils.register_class(ExportSomeData)
def unregister():
    bpy.utils.unregister_class(ExportSomeData)

if __name__ == "__main__":
    register()
    bpy.ops.export_test.some_data('INVOKE_DEFAULT')