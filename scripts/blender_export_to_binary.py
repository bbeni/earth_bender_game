"""
A simple Mesh and Animation file format 

Copyright 2024 Benjamin Froelich 

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
associated documentation files (the “Software”), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial 
portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN 
NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

"""

import bpy
import mathutils
import time
import math
import struct
import bmesh


FILE_MAGIC = b'\xba\xda\xba\xda'
VERSION_NUMBER = 2

class Start_Frame_Bigger_Than_End_Frame_Error(Exception):
    pass

def write_some_data(context, filepath, convert_to_tris, world_space, rot_x90, frames_start, frames_end):
        
    if len(bpy.context.selected_objects) == 0:
        raise ValueError("No objects selected! Select the objects you want to export first\nHave a nice Day <3 :)")
        #return {"CANCELLED"}

    bpy.ops.object.mode_set(mode='OBJECT')
    
    # copy all the selected objects to leave the original intact
    bpy.ops.object.duplicate()    
    objects = bpy.context.selected_objects
        
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.object.mode_set(mode='OBJECT')
    
    # @Temporary just use the first objectp
    obj = objects[0]
    
    if convert_to_tris:
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.quads_convert_to_tris()
        bpy.ops.object.mode_set(mode='OBJECT')
    if world_space:
        obj.data.transform(obj.matrix_world)
    if rot_x90:
        mat_x90 = mathutils.Matrix.Rotation(-math.pi/2, 4, 'X')
        obj.data.transform(mat_x90)
        
    if frames_start > frames_end:
        raise Start_Frame_Bigger_Than_End_Frame_Error

    n_frames = frames_end - frames_start + 1
    
    scene = bpy.context.scene

    #
    # Example
    # one letter coressponds to one byte here
    #

    # xxxx filemagic
    # iiii version number
    
    # b___ do we only have tris? flags
    
    # .... reserved for more flags
    # .... reserved for more flags
    # .... reserved for more flags
    # .... reserved for more flags
    
    # iiii frame count (is 1 for no animation)
    # iiii faces count per frame (needs be constant over all frames)
    
    # per frame we have:
    #   per face we have:
    #     iiii material index
    #     ffff x normal
    #     ffff y normal
    #     ffff z normal
    #     ffff x1
    #     ffff y1
    #     ffff z1
    #     ffff x2
    #     ffff y2
    #     ffff z2
    #     ffff x3
    #     ffff y3
    #     ffff z3
    #     ffff u1
    #     ffff v1
    #     ffff u2
    #     ffff v2
    #     ffff u3
    #     ffff v3
    
    # xxxx filemagic again

    with open(filepath, 'wb') as f:

        # python reference struct pckage        
        # i: 4 byte int, h: 2 byte short, b: 1 byte, x: pad 1 byte, q: 8 bytes,
        # d: 8 byte, f: 4 bytes

        # File magic
        f.write(FILE_MAGIC)
        f.write(struct.pack('<i', VERSION_NUMBER))
        
        # did we convert to tris? (one flag for now)
        f.write(struct.pack('<bxxx', convert_to_tris))

        # reserved for other flags etc..
        f.write(struct.pack('<i', 0))
        f.write(struct.pack('<i', 0))
        f.write(struct.pack('<i', 0))
        f.write(struct.pack('<i', 0))

        # amount of frames (1 if we have no animation)
        f.write(struct.pack('<i', n_frames))


        # just take the number of faces we have and assume its not 
        # gonna change!
        me = obj.data
        bm = bmesh.new()
        bm.from_mesh(me)
        faces = list(bm.faces)
        len_faces = len(faces)

        # write amount of faces per frame needs to be constant
        f.write(struct.pack('<i', len_faces))
                
        # per frame do this        
        for current_frame in range(frames_start, frames_end + 1):
            
            scene.frame_set(current_frame)
            bpy.context.view_layer.update()  # Make sure all transforms are applied
            
            me = obj.evaluated_get(bpy.context.evaluated_depsgraph_get()).to_mesh()
            bm = bmesh.new()   # create an empty BMesh
            bm.from_mesh(me)   # fill it in from a Mesh

            faces = list(bm.faces)
            assert(len(faces) == len_faces) # make sure it didnt change
            
            uv_lay = bm.loops.layers.uv.active            
            
            for face in faces:
                loops = list(face.loops)

                if convert_to_tris:
                    assert len(loops) == 3

                # write material index
                f.write(struct.pack("<i", face.material_index))

                # we write normal x, y, z
                f.write(struct.pack("<fff", *face.normal))
                #print("Normal: ", face.normal)          

                # positions x1, y1, z1, x2, y2, y2, ...
                for i in range(len(loops)):
                    f.write(struct.pack("<fff", *loops[i].vert.co[:]))
                    #print("Vert: ", loops[i].vert.co)

                # uv coords u1, v1,     u2, v2,     ...
                for i in range(len(loops)):
                    f.write(struct.pack("<ff", *loops[i][uv_lay].uv))        
                    #print("Vert: ", loops[i][uv_lay].uv)

        # File identifier again for signifing we won
        f.write(FILE_MAGIC)
    
    # cleanup the copyied objects from before
    bpy.ops.object.delete()
    
    return {'FINISHED'}

from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, IntVectorProperty, IntProperty
from bpy.types import Operator     
   

class ExportBadaFile(Operator, ExportHelper):
    """Export selected mesh(es) to .bada file (binary)."""
    bl_idname = "export_test.some_data"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export To .bada"

    # ExportHelper mixin class uses this
    filename_ext = ".bada"

    filter_glob: StringProperty(
        default="*.bada",
        options={'HIDDEN'},
        maxlen=255,  # Max internal buffer length, longer would be clamped.
    )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    convert_tris: BoolProperty(
        name="To Tris",
        description="Convert Quads To Tris",
        default=True,
    )
    
    to_world_space: BoolProperty(
        name="Convert To Worldspace",
        description="Convert the Mesh from local to Worldspace",
        default=False,
    )
    
    rotate90: BoolProperty(
        name="Rotate x by 90 degrees",
        description="Rotate it by 90 degrees around X-axis",
        default=False
    )

    frames_start: IntProperty(
        name="start frame",
        description="set start = end for no animation",
        default=bpy.context.scene.frame_start,
        min=bpy.context.scene.frame_start,
        max=bpy.context.scene.frame_end,
    )

    frames_end: IntProperty(
        name="end frame",
        description="set start = end for no animation",
        default=bpy.context.scene.frame_end,
        min=bpy.context.scene.frame_start,
        max=bpy.context.scene.frame_end,
    )
    

    def execute(self, context):
        return write_some_data(context, self.filepath, self.convert_tris, self.to_world_space, self.rotate90, self.frames_start, self.frames_end)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportBadaFile.bl_idname, text="Export Bada (.bada)")


def register():
    bpy.utils.register_class(ExportBadaFile)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportBadaFile)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    # bpy.ops.export_test.some_data('INVOKE_DEFAULT')