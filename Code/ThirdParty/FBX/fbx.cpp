#include "Thirdparty/FBX/fbx.hpp"

#if defined(TOOLS_BUILD) 
// If we have FBX IMPORTING ENABLED, make sure to include the library and header files.
//#define __PLACEMENT_NEW_INLINE 

#include <algorithm>
#include <string>
#include <sstream>
#include <utility>

#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk-md.lib")

#include "Engine/EngineConfig.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/MatrixStack.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/MeshSkeleton.hpp"
#include "Engine/Renderer/MeshMotion.hpp"

#include "Engine/RHI/RHITypes.hpp"


//------------------------------------------------------------------------
// helper struct for loading
struct fbx_skin_weight_t {
    fbx_skin_weight_t():
        indices(IntVector4::ZERO),
        weights(Vector4::ZERO)
    { /* DO NOTHING */ }

    void reset() {
        indices = IntVector4::ZERO;
        weights = Vector4::ZERO;
    }

    void root() {
        indices = IntVector4::ZERO;
        weights = Vector4::X_AXIS;
    }

    IntVector4 indices;
    Vector4 weights;
};

char const* GetAttributeTypeName(fbxsdk::FbxNodeAttribute::EType type);
void ConvertSceneToEngineBasis(fbxsdk::FbxScene* scene);
void TriangulateScene(fbxsdk::FbxScene* scene);
void FlipX(MeshBuilder* mb);
void FlipXAxis(MeshSkeleton* skel);
void ImportMeshes(MeshBuilder *out, fbxsdk::FbxNode *node, MatrixStack& mat_stack, MeshSkeleton* skeleton = nullptr);
Matrix4 GetNodeWorldTransform(fbxsdk::FbxNode *node);
Matrix4 ToEngineMatrix(const fbxsdk::FbxMatrix& m);
static void ImportMesh(MeshBuilder* out, MatrixStack& mat_stack, fbxsdk::FbxMesh* mesh, const MeshSkeleton* skeleton = nullptr);
Matrix4 GetGeometricTransform(fbxsdk::FbxNode *node);
void FbxImportVertex(MeshBuilder* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);
Matrix4 GetNodeWorldTransformAtTime(fbxsdk::FbxNode *node, fbxsdk::FbxTime time);
fbxsdk::FbxPose* GetBindPose(fbxsdk::FbxScene* scene);
void ImportSkeleton(MeshSkeleton* out, fbxsdk::FbxNode* node, fbxsdk::FbxSkeleton* root_bone, fbxsdk::FbxSkeleton* parent_bone, fbxsdk::FbxPose* pose);
static std::string GetNodeName(const fbxsdk::FbxNode* node);
static std::string GetBoneName(const fbxsdk::FbxSkeleton* skel);
void FbxImportVertex(MeshBuilder* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx, const std::vector<fbx_skin_weight_t>& skin_weights);
bool GetNormal(Vector3* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);
bool GetTangent(Vector3* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);
bool GetBitangent(Vector3* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);
bool GetPosition(Vector3* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);
bool GetUV(Vector2* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);
fbxsdk::FbxScene* FbxLoadScene(char const *filename, bool import_embedded);
void FbxUnloadScene(fbxsdk::FbxScene *scene);
void FbxPrintNode(fbxsdk::FbxNode *node, int depth);
void FbxListScene(fbxsdk::FbxScene *scene);
void FbxPrintAttribute(fbxsdk::FbxNodeAttribute* pAttribute, int depth);
static bool FbxImportMotion(MeshMotion* motion, const MeshSkeleton* skeleton, fbxsdk::FbxScene* scene, fbxsdk::FbxAnimStack* anim, float framerate);
Vector4 ToVec4(const fbxsdk::FbxVector4& fbxVec4);
Vector2 ToVec2(const fbxsdk::FbxVector2& fbxVec2);
fbxsdk::FbxPose* GetBindPose(fbxsdk::FbxScene* scene);
void ImportSkeleton(MeshSkeleton* out, fbxsdk::FbxNode* node, fbxsdk::FbxSkeleton* root_bone, fbxsdk::FbxSkeleton* parent_bone, fbxsdk::FbxPose* pose);
static std::string GetNodeName(const fbxsdk::FbxNode* node);
static std::string GetBoneName(const fbxsdk::FbxSkeleton* skel);
void CalculateSkinWeights(std::vector<fbx_skin_weight_t>& skin_weights, const FbxMesh* mesh, const MeshSkeleton* skeleton);
void AddHighestWeight(fbx_skin_weight_t& skin_weight, unsigned int bone_idx, float weight);
bool HasSkinWeights(const fbxsdk::FbxMesh* mesh);
void NormalizeSkinWeights(std::vector<fbx_skin_weight_t>& skin_weights);
void SetSkinWeightsToRoot(std::vector<fbx_skin_weight_t>& skin_weights);
bool GetBoneIndices(IntVector4* out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);
bool GetBoneWeights(Vector4* skin_weights, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx);

Matrix4 ToEngineMatrix(const fbxsdk::FbxMatrix& m) {

    Matrix4 const flip_x = Matrix4(
        -Vector3::X_AXIS,
        Vector3::Y_AXIS,
        Vector3::Z_AXIS);

    fbxsdk::FbxDouble4 row0 = m.mData[0];
    fbxsdk::FbxDouble4 row1 = m.mData[1];
    fbxsdk::FbxDouble4 row2 = m.mData[2];
    fbxsdk::FbxDouble4 row3 = m.mData[3];

    Matrix4 ret = Matrix4(
        Vector4((float)row0[0], (float)row0[1], (float)row0[2], (float)row0[3]),
        Vector4((float)row1[0], (float)row1[1], (float)row1[2], (float)row1[3]),
        Vector4((float)row2[0], (float)row2[1], (float)row2[2], (float)row2[3]),
        Vector4((float)row3[0], (float)row3[1], (float)row3[2], (float)row3[3])
    );

    return flip_x * ret * flip_x;
}

bool GetBoneIndices(IntVector4* out, const std::vector<fbx_skin_weight_t>& skinWeights, const Matrix4& /*transform*/, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx) {
    if(HasSkinWeights(mesh)) {
        // First, get the control point index for this poly/vert pair.
        int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
        if(ctrl_idx >= 0) {
            *out = skinWeights[ctrl_idx].indices;
            return true;
        }
    }
    return false;
}
bool GetBoneWeights(Vector4* out, const std::vector<fbx_skin_weight_t>& skinWeights, const Matrix4& /*transform*/, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx) {
    if(HasSkinWeights(mesh)) {
        // First, get the control point index for this poly/vert pair.
        int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
        if(ctrl_idx >= 0) {
            *out = skinWeights[ctrl_idx].weights;
            return true;
        }
    }
    return false;
}
void FbxImportVertex(MeshBuilder *out, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx, const std::vector<fbx_skin_weight_t>& skin_weights) {
    /** NEW BITS **/

    Vector2 uv;
    if(GetUV(&uv, transform, mesh, poly_idx, vert_idx)) {
        out->SetUV(uv);
    }

    // Will demonstarate with normal, but this process is repeated for
    // uvs, colors, tangents, bitangents (called binormals in the SDK)
    Vector3 normal;
    if(GetNormal(&normal, transform, mesh, poly_idx, vert_idx)) {
        out->SetNormal(normal);
    }

    Vector3 tangent;
    if(GetTangent(&tangent, transform, mesh, poly_idx, vert_idx)) {
        out->SetTangent(tangent);
    }

    Vector3 bitangent;
    if(GetBitangent(&bitangent, transform, mesh, poly_idx, vert_idx)) {
        out->SetBitangent(bitangent);
    }

    if(!skin_weights.empty() && skin_weights.size() > static_cast<unsigned int>(vert_idx)) {
        Vector4 bone_weights;
        IntVector4 bone_indicies;
        if(GetBoneWeights(&bone_weights, skin_weights, transform, mesh, poly_idx, vert_idx)) {
            out->SetBoneWeights(bone_weights);
        }
        if(GetBoneIndices(&bone_indicies, skin_weights, transform, mesh, poly_idx, vert_idx)) {
            out->SetBoneIndices(bone_indicies);
        }
    }
    Vector3 position;
    if(GetPosition(&position, transform, mesh, poly_idx, vert_idx)) {
        out->AddVertex(position);
    }
}

Vector4 ToVec4(const fbxsdk::FbxVector4& fbxVec4) {
    return Vector4((float)fbxVec4.mData[0],
        (float)fbxVec4.mData[1],
                   (float)fbxVec4.mData[2],
                   (float)fbxVec4.mData[3]);
}

Vector2 ToVec2(const fbxsdk::FbxVector2& fbxVec4) {
    return Vector2((float)fbxVec4.mData[0],
                   (float)fbxVec4.mData[1]
                  );
}

bool GetNormal(Vector3* out, const Matrix4& /*transform*/, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx) {
    using namespace fbxsdk;

    // First, we need to get the geometry element we care about...
    // you can this of this as a "layer" of data for a mesh.  In this case
    // is is the normal data.

    // Note:  0 Index - meshes can potentially contain multiple layers
    // of the same type (UVs are the most common example of this).  
    // For Normals, I just use the first one and continue on.
    FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
    if(element == nullptr) {
        // no layer - no data
        return false;
    }

    // Next, we need to figure out how to pull the normal for this particular vertex out
    // of this layer. 
    // 
    // So, vnormal is just a packed array of normals, or potentially normals, and
    // and index buffer into those normals.
    //
    // Either way, you can think of vnormal as a packed collection of normals.
    // and we're trying to figure out which element of this array we want (elem_idx)

    // So first, get the element index.
    // (Is the data tied to the control point, or the polygon vertex)
    int elem_idx = 0;
    switch(element->GetMappingMode()) {
        case FbxGeometryElement::eByControlPoint:
        {
            // Get the normal by control point - most direct way.
            elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
        } break; // case: eByControlPoint

        case FbxGeometryElement::eByPolygonVertex:
        {
            // array is packed by polygon vertex - so each polygon is unique
            // (think sharp edges in the case of normals)
            elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
        } break; // case: eByPolygonVertex

        default:
            ERROR_AND_DIE("GetNormal: Unknown Mapping."); // Unknown mapping mode
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch(element->GetReferenceMode()) {
        case FbxGeometryElement::eDirect:
        {
            // this is just an array - use elem index as an index into this array
            if(elem_idx < element->GetDirectArray().GetCount()) {
                *out = Vector3(ToVec4(element->GetDirectArray().GetAt(elem_idx)));
                return true;
            }
        } break;

        case FbxGeometryElement::eIndexToDirect:
        {
            // This is an indexed array, so elem_idx is our offset into the 
            // index buffer.  We use that to get our index into the direct array.
            if(elem_idx < element->GetIndexArray().GetCount()) {
                int index = element->GetIndexArray().GetAt(elem_idx);
                *out = Vector3(ToVec4(element->GetDirectArray().GetAt(index)));
                return true;
            }
        } break;

        default:
            ERROR_AND_DIE("GetNormal: Unknown reference type."); // Unknown reference type
    }
    return false;
}

bool GetTangent(Vector3* out, const Matrix4& /*transform*/, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx) {
    using namespace fbxsdk;

    // First, we need to get the geometry element we care about...
    // you can this of this as a "layer" of data for a mesh.  In this case
    // is is the normal data.

    // Note:  0 Index - meshes can potentially contain multiple layers
    // of the same type (UVs are the most common example of this).  
    // For Normals, I just use the first one and continue on.
    FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
    if(element == nullptr) {
        // no layer - no data
        return false;
    }

    // Next, we need to figure out how to pull the normal for this particular vertex out
    // of this layer. 
    // 
    // So, vnormal is just a packed array of normals, or potentially normals, and
    // and index buffer into those normals.
    //
    // Either way, you can think of vnormal as a packed collection of normals.
    // and we're trying to figure out which element of this array we want (elem_idx)

    // So first, get the element index.
    // (Is the data tied to the control point, or the polygon vertex)
    int elem_idx = 0;
    switch(element->GetMappingMode()) {
        case FbxGeometryElement::eByControlPoint:
        {
            // Get the normal by control point - most direct way.
            elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
        } break; // case: eByControlPoint

        case FbxGeometryElement::eByPolygonVertex:
        {
            // array is packed by polygon vertex - so each polygon is unique
            // (think sharp edges in the case of normals)
            elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
        } break; // case: eByPolygonVertex

        default:
            ERROR_AND_DIE("GetNormal: Unknown Mapping."); // Unknown mapping mode
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch(element->GetReferenceMode()) {
        case FbxGeometryElement::eDirect:
        {
            // this is just an array - use elem index as an index into this array
            if(elem_idx < element->GetDirectArray().GetCount()) {
                *out = Vector3(ToVec4(element->GetDirectArray().GetAt(elem_idx)));
                return true;
            }
        } break;

        case FbxGeometryElement::eIndexToDirect:
        {
            // This is an indexed array, so elem_idx is our offset into the 
            // index buffer.  We use that to get our index into the direct array.
            if(elem_idx < element->GetIndexArray().GetCount()) {
                int index = element->GetIndexArray().GetAt(elem_idx);
                *out = Vector3(ToVec4(element->GetDirectArray().GetAt(index)));
                return true;
            }
        } break;

        default:
            ERROR_AND_DIE("GetNormal: Unknown reference type."); // Unknown reference type
    }
    return false;
}

bool GetBitangent(Vector3* out, const Matrix4& /*transform*/, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx) {
    using namespace fbxsdk;

    // First, we need to get the geometry element we care about...
    // you can this of this as a "layer" of data for a mesh.  In this case
    // is is the normal data.

    // Note:  0 Index - meshes can potentially contain multiple layers
    // of the same type (UVs are the most common example of this).  
    // For Normals, I just use the first one and continue on.
    FbxGeometryElementNormal *element = mesh->GetElementNormal(0);
    if(element == nullptr) {
        // no layer - no data
        return false;
    }

    // Next, we need to figure out how to pull the normal for this particular vertex out
    // of this layer. 
    // 
    // So, vnormal is just a packed array of normals, or potentially normals, and
    // and index buffer into those normals.
    //
    // Either way, you can think of vnormal as a packed collection of normals.
    // and we're trying to figure out which element of this array we want (elem_idx)

    // So first, get the element index.
    // (Is the data tied to the control point, or the polygon vertex)
    int elem_idx = 0;
    switch(element->GetMappingMode()) {
        case FbxGeometryElement::eByControlPoint:
        {
            // Get the normal by control point - most direct way.
            elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
        } break; // case: eByControlPoint

        case FbxGeometryElement::eByPolygonVertex:
        {
            // array is packed by polygon vertex - so each polygon is unique
            // (think sharp edges in the case of normals)
            elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
        } break; // case: eByPolygonVertex

        default:
            ERROR_AND_DIE("GetNormal: Unknown Mapping."); // Unknown mapping mode
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch(element->GetReferenceMode()) {
        case FbxGeometryElement::eDirect:
        {
            // this is just an array - use elem index as an index into this array
            if(elem_idx < element->GetDirectArray().GetCount()) {
                *out = Vector3(ToVec4(element->GetDirectArray().GetAt(elem_idx)));
                return true;
            }
        } break;

        case FbxGeometryElement::eIndexToDirect:
        {
            // This is an indexed array, so elem_idx is our offset into the 
            // index buffer.  We use that to get our index into the direct array.
            if(elem_idx < element->GetIndexArray().GetCount()) {
                int index = element->GetIndexArray().GetAt(elem_idx);
                *out = Vector3(ToVec4(element->GetDirectArray().GetAt(index)));
                return true;
            }
        } break;

        default:
            ERROR_AND_DIE("GetNormal: Unknown reference type."); // Unknown reference type
    }
    return false;
}

float GetNativeFramerate(const fbxsdk::FbxScene* scene) {
    using namespace fbxsdk;

    // Get the scenes authored framerate
    const FbxGlobalSettings& settings = scene->GetGlobalSettings();
    FbxTime::EMode time_mode = settings.GetTimeMode();
    double framerate;
    if(time_mode == FbxTime::eCustom) {
        framerate = settings.GetCustomFrameRate();
    } else {
        framerate = FbxTime::GetFrameRate(time_mode);
    }
    return (float)framerate;
}

bool FbxImportMotion(MeshMotion* motion, const MeshSkeleton* skeleton, fbxsdk::FbxScene* scene, fbxsdk::FbxAnimStack* anim, float framerate) {
    using namespace fbxsdk;

    // First, set the scene to use this animation - helps all the evaluation functions
    scene->SetCurrentAnimationStack(anim);

    // how far do we move through this
    FbxTime advance;
    advance.SetSecondDouble((double)(1.0f / framerate));

    // Get the start, end, and duration of this animation.
    FbxTime local_start = anim->LocalStart;
    FbxTime local_end = anim->LocalStop;
    float local_start_fl = (float)local_start.GetSecondDouble();
    float local_end_fl   = (float)local_end.GetSecondDouble();

    // Okay, so local start will sometimes start BEFORE the first frame - probably to give it something to T
    // pose?  This was adding a jerky frame movement to UnityChan's walk cycle. 
    // Whatever, we'll always start at least 0.
    local_start_fl = (std::max)(0.0f, local_start_fl);
    float duration_fl = local_end_fl - local_start_fl;

    // no duration, no animation!
    if(duration_fl <= 0.0f) {
        return false;
    }

    // Alright, we have a motion, we start getting data from it
    char const *motion_name = anim->GetName();
    //float time_span = duration_fl;

    // set some identifying information
    motion->set_framerate(framerate);
    motion->set_name(motion_name);
    motion->set_duration(duration_fl);

    // number of frames should encompasses the duration, so say we are at 10hz,
    // but have an animation that is 0.21f seconds.  We need at least...
    //    3 intervals (Ceiling(10 * 0.21) = Ceiling(2.1) = 3)
    //    4 frames (0.0, 0.1, 0.2, 0.3)
    unsigned int frame_count = (unsigned int)std::ceil(duration_fl * framerate) + 1;


    // Now, for each joint in our skeleton
    // not the most efficient way to go about this, but whatever - tool step
    // and it is not slow enough to matter.
    unsigned int joint_count = skeleton->get_joint_count();
    for(unsigned int joint_idx = 0; joint_idx < joint_count; ++joint_idx) {
        auto joint_name = skeleton->get_joint_name(joint_idx);
        fbxsdk::FbxString fbxName = fbxsdk::FbxString(joint_name.c_str());
        // get the node associated with this bone
        // I rely on the names to find this - so author of animations
        // should be sure to be using the same rig/rig names.
        // (this is only important for the import process, after that
        // everything is assuming matching indices)
        FbxNode *node = scene->FindNodeByName(fbxName);
        if(node == nullptr) {
            return false;
        }

        FbxNode *parent = nullptr;
        unsigned int parent_idx = skeleton->get_joint_count();
        if(skeleton && skeleton->get_joint_parent(joint_idx)) {
            parent_idx = skeleton->get_joint_index(skeleton->get_joint_parent(joint_idx)->name);
        }

        // get the parent node
        if(parent_idx != joint_count) {
            auto parent_name = skeleton->get_joint_name(parent_idx);
            fbxsdk::FbxString fbxParent = fbxsdk::FbxString(parent_name.c_str());
            parent = scene->FindNodeByName(fbxParent);
            if(parent == nullptr) {
                return false;
            }
        }

        // Now, for this entire animation, evaluate the local transform for this bone at every interval
        // number of frames is 
        FbxTime eval_time = FbxTime(0);
        for(unsigned int frame_idx = 0; frame_idx < frame_count; ++frame_idx) {
            // Okay, get the pose we want
            MeshPose& pose = motion->get_pose(frame_idx);
            Matrix4 joint_world = GetNodeWorldTransformAtTime(node, eval_time);
            Matrix4 joint_local = joint_world;
            if(nullptr != parent) {
                Matrix4 parent_world = GetNodeWorldTransformAtTime(parent, eval_time);
                joint_local = Matrix4::CalculateInverse(parent_world) * joint_world;
            }

            pose.local_transforms.push_back(joint_local);

            //------------------------------------------------------------------------
            // Missed code - make sure to advanced the clock
            eval_time += advance;
            //------------------------------------------------------------------------
        }
    }

    return true;
}
bool FbxLoadMotion(MeshMotion* motion, const MeshSkeleton* skeleton, const std::string& filename, unsigned int framerate /*= 10*/) {
    using namespace fbxsdk;

    // Normal things
    FbxScene *scene = FbxLoadScene(filename.c_str(), false);
    if(nullptr == scene) {
        return false;
    }
    ConvertSceneToEngineBasis(scene);
    

    // Framerate - how often are we sampling this
    float fr = (float)framerate;


    // Next get the animation count.
    unsigned int anim_count = scene->GetSrcObjectCount<FbxAnimStack>();
    if(anim_count > 0U) {
        // great, we have a motion
        // NOTE:  Multiple motions may be embedded in this file, you could update to extract all 
        //for(unsigned int anim_idx = 0; anim_idx < anim_count; ++anim_idx) {
            FbxAnimStack *anim = scene->GetSrcObject<FbxAnimStack>(0);
            if(!FbxImportMotion(motion, skeleton, scene, anim, fr)) {
                std::string msg = "FAILED TO LOAD MOTION ";
                msg += std::to_string(0);
                g_theConsole->WarnMsg(msg);
            }
        //}
    }

    FbxUnloadScene(scene);

    return (motion->get_duration() > 0.0f);
}
Matrix4 GetNodeWorldTransformAtTime(FbxNode *node, FbxTime time) {
    using namespace fbxsdk;

    if(nullptr == node) {
        return Matrix4::GetIdentity();
    }

    FbxMatrix fbx_mat = node->EvaluateGlobalTransform(time);
    return ToEngineMatrix(fbx_mat);
}

//------------------------------------------------------------------------
// Reference:
//    $(FBXSDK_DIR)\samples\Common\Common.cxx
FbxScene* FbxLoadScene(char const *filename, bool import_embedded)
{
    using namespace fbxsdk;

    // Manager - think of it as a running process of the FbxSdk - you could only ever load one of these
    // and call it good - though I usually just keep one around per import.
    FbxManager *fbx_manager = FbxManager::Create();
    if (nullptr == fbx_manager) {
        ERROR_RECOVERABLE("Could not create FBX Manager.");
        return nullptr;
    }

    // Next, set the IO settings - this is how we configure what we're importing
    // By default, we import everything, but if you wanted to explicitly not import some parts
    // you could do that here)
    FbxIOSettings *io_settings = FbxIOSettings::Create(fbx_manager, IOSROOT);
    io_settings->SetBoolProp(IMP_FBX_EXTRACT_EMBEDDED_DATA, import_embedded);

    fbx_manager->SetIOSettings(io_settings);


    // Next, create our importer (since we're loading an FBX, not saving one)
    // Second argument is the name - we don't need to name it.
    FbxImporter *importer = FbxImporter::Create(fbx_manager, "");

    bool result = importer->Initialize(filename,
        -1, // File format, -1 will let the program figure it out
        io_settings);

    if (result) {
        // First, create a scene to be imported into
        // Doesn't need a name, we're not going to do anything with it
        FbxScene *scene = FbxScene::Create(fbx_manager, "");

        // Import into the scene
        result = importer->Import(scene);
        importer->Destroy();
        importer = nullptr;

        if (result) {
            return scene;
        }
    }

    // Failed somewhere, so clean up after ourselves.
    FBX_SAFE_DESTROY(io_settings);
    FBX_SAFE_DESTROY(importer);
    FBX_SAFE_DESTROY(fbx_manager);

    return nullptr;
}

//------------------------------------------------------------------------
void FbxUnloadScene(fbxsdk::FbxScene* scene)
{
    using namespace fbxsdk;

    if (nullptr == scene) {
        return;
    }

    FbxManager *manager = scene->GetFbxManager();
    FbxIOSettings *io_settings = manager->GetIOSettings();

    FBX_SAFE_DESTROY(scene);
    FBX_SAFE_DESTROY(io_settings);
    FBX_SAFE_DESTROY(manager);
}

//------------------------------------------------------------------------
void FbxPrintNode(fbxsdk::FbxNode *node, int depth)
{
    using namespace fbxsdk;

    // Print the node's attributes.

    std::ostringstream ss;
    for (int i = 0; i < depth; ++i) {
        ss << ' ';
    }
    ss << "Node " << node->GetName() << '\n';
    g_theConsole->NotifyMsg(ss.str());
    ss.str("");

    for (int i = 0; i < node->GetNodeAttributeCount(); i++) {
        FbxPrintAttribute(node->GetNodeAttributeByIndex(i), depth);
    }

    // Print the nodes children
    for (int32_t i = 0; i < node->GetChildCount(); ++i) {
        FbxPrintNode(node->GetChild(i), depth + 1);
    }
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool FbxLoadMesh(MeshBuilder *mb, char const *filename)
{
    // First - load the scene - code is identical to when you 
    // listed the file
    FbxScene *scene = FbxLoadScene(filename, false);
    if(nullptr == scene) {
        return false;
    }

    // Second, we want to convert the basis for consitancy
    ConvertSceneToEngineBasis(scene);

    // Third, our engine only deals with triangles, 
    // but authoring programs tend to deal with a lot of 
    // different surface patch types.  For we triangulate
    // (the process of converting every non-triangle polygon to triangles )
    TriangulateScene(scene);


    // Alright, now we can start importing data, for this we'll need a matrix stack 
    // [well, it helps, you can do this without since we'll mostly be deailing with 
    // global transforms outside of what is called the "geometric transform" which is
    // always local]

    // clear all old data before importing
    mb->Clear();

    // My matrix stack treats the top as being IDENTITY
    MatrixStack mat_stack;

    // Next, get the root node, and start importing
    FbxNode *root = scene->GetRootNode();

    // import data into our builder
    ImportMeshes(mb, root, mat_stack);

    // Clean up after ourself
    FbxUnloadScene(scene);


    // [HACK] After loading, I flip everything across the X axis
    // for conistancy.  Would love this to be part of ConvertSceneToEngineBasis,
    // but no mattter the transform I sent, it was always flipped on X [at best]
    //
    // Doing this causes it to be match Unity/Unreal/Editor in terms of orientation
    FlipX(mb);
    return true;
}

//------------------------------------------------------------------------
void FbxListScene(fbxsdk::FbxScene* scene)
{
    using namespace fbxsdk;

    // Print the node
    FbxNode *root = scene->GetRootNode();
    FbxPrintNode(root, 0);
}

//------------------------------------------------------------------------
void FbxPrintAttribute(fbxsdk::FbxNodeAttribute* pAttribute, int depth)
{
    using namespace fbxsdk;

    if (nullptr == pAttribute) {
        return;
    }

    FbxNodeAttribute::EType type = pAttribute->GetAttributeType();

    char const* typeName = GetAttributeTypeName(type);
    char const* attrName = pAttribute->GetName();

    std::ostringstream ss;
    for (int i = 0; i < depth; ++i) {
        ss << ' ';
    }
    ss << "- type=\'" << typeName << "\' name=\'" << attrName << "\'\n";
    g_theConsole->NotifyMsg(ss.str());
    ss.str("");
}

//------------------------------------------------------------------------
char const* GetAttributeTypeName(fbxsdk::FbxNodeAttribute::EType type)
{
    using namespace fbxsdk;

    switch (type) {
    case FbxNodeAttribute::eUnknown: return "unidentified";
    case FbxNodeAttribute::eNull: return "null";
    case FbxNodeAttribute::eMarker: return "marker";
    case FbxNodeAttribute::eSkeleton: return "skeleton";
    case FbxNodeAttribute::eMesh: return "mesh";
    case FbxNodeAttribute::eNurbs: return "nurbs";
    case FbxNodeAttribute::ePatch: return "patch";
    case FbxNodeAttribute::eCamera: return "camera";
    case FbxNodeAttribute::eCameraStereo: return "stereo";
    case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
    case FbxNodeAttribute::eLight: return "light";
    case FbxNodeAttribute::eOpticalReference: return "optical reference";
    case FbxNodeAttribute::eOpticalMarker: return "marker";
    case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
    case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
    case FbxNodeAttribute::eBoundary: return "boundary";
    case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
    case FbxNodeAttribute::eShape: return "shape";
    case FbxNodeAttribute::eLODGroup: return "lodgroup";
    case FbxNodeAttribute::eSubDiv: return "subdiv";
    default: return "unknown";
   }
}

void ConvertSceneToEngineBasis(fbxsdk::FbxScene* scene) {
    fbxsdk::FbxAxisSystem local_system(
        (fbxsdk::FbxAxisSystem::EUpVector) fbxsdk::FbxAxisSystem::EUpVector::eYAxis,
        (fbxsdk::FbxAxisSystem::EFrontVector) fbxsdk::FbxAxisSystem::EFrontVector::eParityOdd,  // by default points toward me.
        (fbxsdk::FbxAxisSystem::ECoordSystem) fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded);

    fbxsdk::FbxAxisSystem scene_system = scene->GetGlobalSettings().GetAxisSystem();
    if(scene_system != local_system) {
        local_system.ConvertScene(scene);
    }
}

void TriangulateScene(fbxsdk::FbxScene* scene) {

    using namespace fbxsdk;

    FbxGeometryConverter converter(scene->GetFbxManager());
    converter.Triangulate(scene, /*replace=*/true);
}

void FlipX(MeshBuilder* mb) {
    for(auto& v : mb->verticies) {
        v.position.x = -v.position.x;
        v.normal.x = -v.normal.x;
        v.tangent.x = -v.tangent.x;
        v.bitangent.x = -v.bitangent.x;
    }
}

void ImportMeshes(MeshBuilder *out, fbxsdk::FbxNode *node, MatrixStack& mat_stack, MeshSkeleton* skeleton /*= nullptr*/) {

    using namespace fbxsdk;

    Matrix4 transform = GetNodeWorldTransform(node);
    mat_stack.push_direct(transform);

    int attrib_count = node->GetNodeAttributeCount();
    for(int ai = 0; ai < attrib_count; ++ai) {
        FbxNodeAttribute* attrib = node->GetNodeAttributeByIndex(ai);
        if(attrib->GetAttributeType() == FbxNodeAttribute::eMesh) {
            ImportMesh(out, mat_stack, (FbxMesh*)attrib, skeleton);
        }
    }

    int child_count = node->GetChildCount();
    for(int ci = 0; ci < child_count; ++ci) {
        FbxNode *child = node->GetChild(ci);
        ImportMeshes(out, child, mat_stack, skeleton);
    }

    mat_stack.pop();

}

Matrix4 GetNodeWorldTransform(fbxsdk::FbxNode *node) {

    using namespace fbxsdk;

    FbxAMatrix fbx_mat = node->EvaluateGlobalTransform();
    return ToEngineMatrix(fbx_mat);
}

void NormalizeSkinWeights(std::vector<fbx_skin_weight_t>& skin_weights) {
    for(auto& skin_weight : skin_weights) {
        float sum = 0.0f;
        sum += skin_weight.weights.x;
        sum += skin_weight.weights.y;
        sum += skin_weight.weights.z;
        sum += skin_weight.weights.w;
        if(!MathUtils::IsEquivalent(sum, 0.0f)) {
            float inv_sum = 1.0f / sum;
            skin_weight.weights.x *= inv_sum;
            skin_weight.weights.y *= inv_sum;
            skin_weight.weights.z *= inv_sum;
            skin_weight.weights.w *= inv_sum;
        } else {
            skin_weight.weights = Vector4::X_AXIS;
            skin_weight.indices = IntVector4::ZERO;
        }
    }
}
void SetSkinWeightsToRoot(std::vector<fbx_skin_weight_t>& skin_weights) {
    for(auto& skin_weight : skin_weights) {
        skin_weight.root();
    }
}
void ImportMesh(MeshBuilder* out, MatrixStack& mat_stack, fbxsdk::FbxMesh* mesh, const MeshSkeleton* skeleton /*= nullptr*/) {
    // Should have been triangulated before this - sort of a late check 
    // [treat this as an ASSERT_OR_DIE]
    ASSERT_RECOVERABLE(mesh->IsTriangleMesh(), "Import Mesh: mesh is not composed of triangles.");


    int poly_count = mesh->GetPolygonCount();
    if(poly_count <= 0) {
        return;
    }

    std::vector<fbx_skin_weight_t> skin_weights;
    if(skeleton != nullptr) {
        if(HasSkinWeights(mesh)) {
            CalculateSkinWeights(skin_weights, mesh, skeleton);
            NormalizeSkinWeights(skin_weights);
        }
    }

    // Geometric Transformations only apply to the current node
    // http://download.autodesk.com/us/fbx/20112/FBX_SDK_HELP/index.html?url=WS1a9193826455f5ff1f92379812724681e696651.htm,topicNumber=d0e7429
    Matrix4 geo_trans = GetGeometricTransform(mesh->GetNode());
    mat_stack.push(geo_trans);

    // import the mesh data.
    Matrix4 transform = mat_stack.top();

    // Starting a draw call.
    out->Begin(PrimitiveType::TRIANGLES, true);

    // Load in the mesh - first, figure out how many polygons there are
    for(int32_t poly_idx = 0; poly_idx < poly_count; ++poly_idx) {

        // For each polygon - get the number of vertices that make it up (should always be 3 for a triangulated mesh)
        int32_t vert_count = mesh->GetPolygonSize(poly_idx);
        ASSERT_OR_DIE(vert_count == 3, "Triangles do not have 3 verticies."); // Triangle Meshes should ALWAYS have 3 verts per poly

        // Finally, import all the data for this vertex (for now, just position)
        for(int32_t vert_idx = 0; vert_idx < vert_count; ++vert_idx) {
            FbxImportVertex(out, transform, mesh, poly_idx, vert_idx, skin_weights);
            out->indicies.push_back(poly_idx * vert_count + vert_idx);
        }
    }

    out->End();

    // pop the geometric transform
    mat_stack.pop();
}

void CalculateSkinWeights(std::vector<fbx_skin_weight_t>& skin_weights, const FbxMesh* mesh, const MeshSkeleton* skeleton) {
    
    using namespace fbxsdk;

    unsigned int ctrl_count = static_cast<unsigned int>(mesh->GetControlPointsCount());

    skin_weights.resize(ctrl_count);
    for(unsigned int i = 0; i < ctrl_count; ++i) {
        skin_weights[i].reset();
    }

    if(skeleton == nullptr) {
        skin_weights.clear();
        return;
    }


    int deformer_count = mesh->GetDeformerCount(FbxDeformer::eSkin);
    for(int didx = 0; didx < deformer_count; ++didx) {
        FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(didx, FbxDeformer::eSkin);
        if(skin == nullptr) {
            continue;
        }

        int cluster_count = skin->GetClusterCount();
        for(int cidx = 0; cidx < cluster_count; ++cidx) {
            FbxCluster* cluster = skin->GetCluster(cidx);
            const FbxNode* link_node = cluster->GetLink();

            if(link_node == nullptr) {
                continue;
            }

            unsigned int joint_idx = skeleton->get_joint_index(link_node->GetName());
            if(joint_idx >= skeleton->get_joint_count()) {
                continue;
            }

            int index_count = cluster->GetControlPointIndicesCount();
            if(index_count == 0) {
                continue;
            }

            int* indices = cluster->GetControlPointIndices();
            double* weights = cluster->GetControlPointWeights();

            if(indices == nullptr || weights == nullptr) {
                continue;
            }

            for(int i = 0; i < index_count; ++i) {
                int control_idx = indices[i];
                double weight = weights[i];
                fbx_skin_weight_t& skin_weight = skin_weights[control_idx];
                AddHighestWeight(skin_weight, joint_idx, (float)weight);
            }
        }
    }

}

//------------------------------------------------------------------------
// Keep track of this influencer if it influences more than an already existing
// bone. [all bones influence at 0 at the start]
void AddHighestWeight(fbx_skin_weight_t& skin_weight, unsigned int bone_idx, float weight) {
    
    float x = skin_weight.weights.x;
    float y = skin_weight.weights.y;
    float z = skin_weight.weights.z;
    float w = skin_weight.weights.w;

    bool x_zero = MathUtils::IsEquivalent(x, 0.0f);
    if(x_zero) {
        skin_weight.weights.x = weight;
        skin_weight.indices.x = bone_idx;
        return;
    }
    bool y_zero = MathUtils::IsEquivalent(y, 0.0f);
    if(y_zero) {
        skin_weight.weights.y = weight;
        skin_weight.indices.y = bone_idx;
        return;
    }
    bool z_zero = MathUtils::IsEquivalent(z, 0.0f);
    if(z_zero) {
        skin_weight.weights.z = weight;
        skin_weight.indices.z = bone_idx;
        return;
    }
    bool w_zero = MathUtils::IsEquivalent(w, 0.0f);
    if(w_zero) {
        skin_weight.weights.w = weight;
        skin_weight.indices.w = bone_idx;
        return;
    }

    if(weight >  x && 
       weight <= y &&
       weight <= z &&
       weight <= w
      ) {
        skin_weight.weights.x = weight;
        skin_weight.indices.x = bone_idx;
    }
    if(weight > y &&
       weight <= x &&
       weight <= z &&
       weight <= w
      ) {
        skin_weight.weights.y = weight;
        skin_weight.indices.y = bone_idx;
    }
    if(weight > z &&
       weight <= x &&
       weight <= y &&
       weight <= w
      ) {
        skin_weight.weights.z = weight;
        skin_weight.indices.z = bone_idx;
    }
    if(weight > w &&
       weight <= x &&
       weight <= y &&
       weight <= z
      ) {
        skin_weight.weights.w = weight;
        skin_weight.indices.w = bone_idx;
    }
}

//------------------------------------------------------------------------
bool HasSkinWeights(const fbxsdk::FbxMesh* mesh) {
    int deformer_count = mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
    return deformer_count > 0;
}



Matrix4 GetGeometricTransform(fbxsdk::FbxNode *node) {

    using namespace fbxsdk;

    Matrix4 ret = Matrix4::GetIdentity();

    if((node != nullptr) && (node->GetNodeAttribute() != nullptr)) {
        FbxEuler::EOrder order;

        node->GetRotationOrder(FbxNode::eSourcePivot, order);

        FbxVector4 const geo_trans = node->GetGeometricTranslation(FbxNode::eSourcePivot);
        FbxVector4 const geo_rot = node->GetGeometricRotation(FbxNode::eSourcePivot);
        FbxVector4 const geo_scale = node->GetGeometricScaling(FbxNode::eSourcePivot);

        FbxAMatrix geo_mat;
        FbxAMatrix mat_rot;
        mat_rot.SetR(geo_rot, order);
        geo_mat.SetTRS(geo_trans, mat_rot, geo_scale);

        ret = ToEngineMatrix(geo_mat);
    }

    return ret;
}

bool GetUV(Vector2* out, const Matrix4& /*transform*/, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx) {
    using namespace fbxsdk;

    FbxGeometryElementUV* element = mesh->GetElementUV(0);
    if(element == nullptr) {
        // no layer - no data
        return false;
    }

    int elem_idx = 0;
    switch(element->GetMappingMode()) {
        case FbxGeometryElement::eByControlPoint:
        {
            // Get the normal by control point - most direct way.
            elem_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
        } break; // case: eByControlPoint

        case FbxGeometryElement::eByPolygonVertex:
        {
            // array is packed by polygon vertex - so each polygon is unique
            // (think sharp edges in the case of normals)
            elem_idx = mesh->GetPolygonVertexIndex(poly_idx) + vert_idx;
        } break; // case: eByPolygonVertex

        default:
            ERROR_AND_DIE("GetUV: Unknown Mapping."); // Unknown mapping mode
    }

    // next, now that we have the element index, we figure out how this is used.
    // Is this a direct array, or an indexed array.
    // If we got an element index, fetch the data based on how it's referenced
    // (Directly, or by an index into an array);
    switch(element->GetReferenceMode()) {
        case FbxGeometryElement::eDirect:
        {
            // this is just an array - use elem index as an index into this array
            if(elem_idx < element->GetDirectArray().GetCount()) {
                *out = ToVec2(element->GetDirectArray().GetAt(elem_idx));
                out->y = 1 - out->y;
                return true;
            }
        } break;

        case FbxGeometryElement::eIndexToDirect:
        {
            // This is an indexed array, so elem_idx is our offset into the 
            // index buffer.  We use that to get our index into the direct array.
            if(elem_idx < element->GetIndexArray().GetCount()) {
                int index = element->GetIndexArray().GetAt(elem_idx);
                *out = ToVec2(element->GetDirectArray().GetAt(index));
                out->y = 1 - out->y;
                return true;
            }
        } break;

        default:
            ERROR_AND_DIE("GetUV: Unknown reference type."); // Unknown reference type
    }
    return false;
}

bool GetPosition(Vector3* out_pos, const Matrix4& transform, fbxsdk::FbxMesh* mesh, int poly_idx, int vert_idx) {
    // First, get the control point index for this poly/vert pair.
    int ctrl_idx = mesh->GetPolygonVertex(poly_idx, vert_idx);
    if(ctrl_idx < 0) {
        return false;
    }

    // Get the position for the control point.
    FbxVector4 fbx_pos = mesh->GetControlPointAt(ctrl_idx);

    // convert to an engine vertex, and save it to the out variable
    Vector4 pos = Vector4(static_cast<float>(fbx_pos.mData[0]), static_cast<float>(fbx_pos.mData[1]), static_cast<float>(fbx_pos.mData[2]), static_cast<float>(fbx_pos.mData[3]));
    pos.CalcHomogeneous();

    Vector4 result = Vector4::CalcHomogeneous(transform * pos);
    out_pos->x = result.x;
    out_pos->y = result.y;
    out_pos->z = result.z;

    // return success.
    return true;
}

void FbxListFile(const char* filename)
{
    using namespace fbxsdk;

    FbxScene *scene = FbxLoadScene(filename, false);
    if(nullptr == scene) {
        DebuggerPrintf("Failed to load scene: %s\n", filename);
        return;
    }
    std::ostringstream ss;
    ss << "Listing File: " << filename << '\n';
    g_theConsole->NotifyMsg(ss.str());
    ss.str("");
    //DebuggerPrintf("Listing File: %s\n", filename);
    FbxListScene(scene);

    FbxUnloadScene(scene);
}
void FbxListFile(const std::string& filename)
{
    FbxListFile(filename.c_str());
}

bool FbxLoadMesh(MeshBuilder& mb, const std::string& filename, MeshSkeleton* skeleton /*= nullptr*/) {

    // First - load the scene - code is identical to when you 
    // listed the file
    FbxScene *scene = FbxLoadScene(filename.c_str(), false);
    if(nullptr == scene) {
        return false;
    }

    // Second, we want to convert the basis for consitancy
    ConvertSceneToEngineBasis(scene);

    // Third, our engine only deals with triangles, 
    // but authoring programs tend to deal with a lot of 
    // different surface patch types.  For we triangulate
    // (the process of converting every non-triangle polygon to triangles )
    TriangulateScene(scene);


    // Alright, now we can start importing data, for this we'll need a matrix stack 
    // [well, it helps, you can do this without since we'll mostly be deailing with 
    // global transforms outside of what is called the "geometric transform" which is
    // always local]

    // clear all old data before importing
    mb.Clear();

    // My matrix stack treats the top as being IDENTITY
    MatrixStack mat_stack;

    // Next, get the root node, and start importing
    FbxNode *root = scene->GetRootNode();

    // import data into our builder
    ImportMeshes(&mb, root, mat_stack, skeleton);

    // Clean up after ourself
    FbxUnloadScene(scene);


    // [HACK] After loading, I flip everything across the X axis
    // for conistancy.  Would love this to be part of ConvertSceneToEngineBasis,
    // but no mattter the transform I sent, it was always flipped on X [at best]
    //
    // Doing this causes it to be match Unity/Unreal/Editor in terms of orientation
    FlipX(&mb);
    return true;
}


void FlipXAxis(MeshSkeleton* skel) {
    for(std::size_t i = 0; i < skel->get_joint_count(); ++i) {
        Matrix4 transform = skel->get_joint_transform(i);
        Vector4 ibasis = transform.GetIBasis();
        ibasis.x *= -1.0f;
        transform.SetIBasis(ibasis);
    }
}
bool FbxLoadSkeleton(MeshSkeleton* skel, const std::string& filename) {

    using namespace fbxsdk;

    // Same as you did for Meshes.
    FbxScene *fbx_scene = FbxLoadScene(filename.c_str(), false);
    if(nullptr == fbx_scene) {
        return false;
    }

    ConvertSceneToEngineBasis(fbx_scene);

    // First, get the bind post for the scene
    skel->clear();


    // Now, traverse the scene, and build the skeleton out with 
    // the bind pose positions
    FbxNode *root = fbx_scene->GetRootNode();

    // Bind pose - the pose which assets are authored for.
    FbxPose *pose = GetBindPose(fbx_scene);

    // Import the skeleton by traversing the scene.
    ImportSkeleton(skel, root, nullptr, nullptr, pose);

    // Flip the Axis 
    FlipXAxis(skel);

    FbxUnloadScene(fbx_scene);

    // Success if I've loaded in at least ONE bone/joint.
    return (skel->get_joint_count() > 0U);
}

FbxPose* GetBindPose(FbxScene* scene) {

    using namespace fbxsdk;

    int pose_count = scene->GetPoseCount();
    for(int i = 0; i < pose_count; ++i) {
        FbxPose *pose = scene->GetPose(i);
        if(pose->IsBindPose()) {
            // DEBUG - Print all nodes part of this pose
            /*
            int count = pose->GetCount();
            for (int j = 0; j < count; ++j) {
            FbxNode *node = pose->GetNode(j);
            Trace( "fbx", "Pose Node: %s", node->GetName() );
            }
            */

            return pose;
        }
    }

    return nullptr;
}

void ImportSkeleton(MeshSkeleton* out, FbxNode* node, FbxSkeleton* root_bone, FbxSkeleton* parent_bone, FbxPose* pose) {

    using namespace fbxsdk;

    for(int i = 0; i < node->GetNodeAttributeCount(); ++i) {

        // If this node is a skeleton node (a joint), 
        // then it should have a skeleton attribute. 
        FbxNodeAttribute *na = node->GetNodeAttributeByIndex(i);
        if(na->GetAttributeType() == FbxNodeAttribute::eSkeleton) {

            // Cast it, and figure out the type of bone. 
            FbxSkeleton *skel = (FbxSkeleton*)na;
            FbxSkeleton::EType type = skel->GetSkeletonType();

            // From FBXSDK Documentation.
            // eRoot,			/*!< First element of a chain. */
            // eLimb,			/*!< Chain element. */
            // eLimbNode,		/*!< Chain element. */
            // eEffector		/*!< Last element of a chain. */

            // If this is a root - we better have not have another root higher 
            // up (for now).  This is just here to catch this happening so
            // I know if I should support it in the future - you could cut this.
            if((type == FbxSkeleton::eRoot) || (type == FbxSkeleton::eEffector)) {
                // this is a root bone - so can treat it as the root
                // of a skeleton.
                // TODO: no support for nested skeletons yet.
                g_theFileLogger->LogFlush();
                ASSERT_OR_DIE(root_bone == nullptr, "Nested skeletons not supported.");
                root_bone = skel;
            } // else, it is a limb or limb node, which we treat the same

              // See if this bone exists int he pose, and if so, 
              // get its index in the pose.
            int pose_node_idx = -1;
            if(pose != nullptr) {
                pose_node_idx = pose->Find(node);
            }

            // Get the global transform for this bone
            Matrix4 bone_transform = Matrix4::GetIdentity();
            if(pose_node_idx != -1) {
                // If it exists in the pose - use the poses transform.
                // Since we're only doing the bind pose now, the pose should ONLY
                // be storing global transforms (as per the documentation).
                //
                // When we update this code to load in animated poses, this willc hange.
                bool is_local = pose->IsLocalMatrix(pose_node_idx);
                g_theFileLogger->LogFlush();
                ASSERT_OR_DIE(false == is_local, "Local matrix transforms not supported."); // for now, should always be global (pose transforms always are)

                FbxMatrix fbx_matrix = pose->GetMatrix(pose_node_idx);
                bone_transform = ToEngineMatrix(fbx_matrix);
            } else {
                // Not in the pose - so we have two options
                if(parent_bone == nullptr) {
                    // We have no parent [first in the skeleton], so just use 
                    // the global transform of the node.
                    bone_transform = ToEngineMatrix(node->EvaluateGlobalTransform());
                } else {
                    // Otherwise, calculate my world based on my parent transform
                    // and my nodes local.
                    //
                    // The reason behind this is EvaluateGlobalTransform actually
                    // gets the animated transform, while getting the Pose transform
                    // will get an unanimated transform.  So if we're a child of a transform
                    // that was gotten from a pose, I should be evaluating off that.
                    //
                    // TODO: Evaluate Local from Parent - just in case their is siblings between this
                    Matrix4 local = ToEngineMatrix(node->EvaluateLocalTransform());
                    Matrix4 ptrans = out->get_joint_transform(GetBoneName(parent_bone));
                    bone_transform = ptrans * local;
                }
            }

            // Add a joint.
            out->add_joint(GetBoneName(skel),
                (parent_bone != nullptr) ? GetBoneName(parent_bone) : std::string(""),
                           bone_transform);

            // set this as the next nodes parent, and continue down the chain
            parent_bone = skel;
            break;
        }
    }

    // import the rest
    for(int i = 0; i < node->GetChildCount(); ++i) {
        ImportSkeleton(out, node->GetChild(i), root_bone, parent_bone, pose);
    }
}
std::string GetNodeName(FbxNode const *node) {
    if(nullptr == node) {
        return std::string("");
    } else {
        auto s = node->GetName();
        return s ? s : std::string("");
    }
}

std::string GetBoneName(const FbxSkeleton* skel) {

    using namespace fbxsdk;

    if(skel == nullptr) {
        return std::string("");
    }

    const std::string& node_name = GetNodeName(skel->GetNode());
    if(!node_name.empty()) {
        return node_name;
    } else {
        return skel->GetName();
    }
}


#else 

// Stubs so that if we don't have the FBX SDK Available, our program does not crash
void FbxListFile(const char* /*filepath*/) { /* DO NOTHING */ }
void FbxListFile(const std::string& filepath) { FbxListFile(filepath.c_str()); }
bool FbxLoadMesh(MeshBuilder& /*mb*/, const std::string& /*filename*/, MeshSkeleton* /*skeleton*/) { return false; }
bool FbxLoadSkeleton(MeshSkeleton* /*skel*/, const std::string& /*filename*/) { return false; }
bool FbxLoadMotion(MeshMotion* /*motion*/, const MeshSkeleton* /*skeleton*/, const std::string& /*filename*/, unsigned int /*framerate*/ /*= 10*/) { return false; }
#endif
