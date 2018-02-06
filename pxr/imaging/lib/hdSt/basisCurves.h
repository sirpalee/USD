//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef HDST_BASIS_CURVES_H
#define HDST_BASIS_CURVES_H

#include "pxr/pxr.h"
#include "pxr/imaging/hdSt/api.h"
#include "pxr/imaging/hd/version.h"
#include "pxr/imaging/hd/basisCurves.h"
#include "pxr/imaging/hd/drawingCoord.h"
#include "pxr/imaging/hd/enums.h"
#include "pxr/imaging/hd/perfLog.h"

#include "pxr/usd/sdf/path.h"
#include "pxr/base/vt/array.h"

#include <boost/shared_ptr.hpp>

PXR_NAMESPACE_OPEN_SCOPE

class HdStDrawItem;
typedef boost::shared_ptr<class HdResourceRegistry> HdResourceRegistrySharedPtr;
typedef boost::shared_ptr<class HdSt_BasisCurvesTopology>
                                              HdSt_BasisCurvesTopologySharedPtr;

/// \class HdStBasisCurves
///
/// A collection of curves using a particular basis.
///
class HdStBasisCurves final: public HdBasisCurves {
public:
    HF_MALLOC_TAG_NEW("new HdStBasisCurves");

    HDST_API
    HdStBasisCurves(SdfPath const& id,
                    SdfPath const& instancerId = SdfPath());
    HDST_API
    virtual ~HdStBasisCurves();

    HDST_API
    virtual void Sync(HdSceneDelegate* delegate,
                      HdRenderParam*   renderParam,
                      HdDirtyBits*     dirtyBits,
                      TfToken const &  reprName,
                      bool             forcedRepr) override;

protected:
    virtual HdReprSharedPtr const &
        _GetRepr(HdSceneDelegate *sceneDelegate,
                 TfToken const &reprName,
                 HdDirtyBits *dirtyBitsState) override;

    void _PopulateTopology(HdSceneDelegate *sceneDelegate,
                           HdStDrawItem *drawItem,
                           HdDirtyBits *dirtyBits,
                           const HdBasisCurvesReprDesc &desc);

    void _PopulateVertexPrimVars(HdSceneDelegate *sceneDelegate,
                                 HdStDrawItem *drawItem,
                                 HdDirtyBits *dirtyBits);

    void _PopulateElementPrimVars(HdSceneDelegate *sceneDelegate,
                                  HdStDrawItem *drawItem,
                                  HdDirtyBits *dirtyBits);


    virtual HdDirtyBits _GetInitialDirtyBits() const override;
    virtual HdDirtyBits _PropagateDirtyBits(HdDirtyBits bits) const override;
    virtual void _InitRepr(TfToken const &reprName,
                           HdDirtyBits *dirtyBits) override;

private:
    enum DrawingCoord {
        HullTopology = HdDrawingCoord::CustomSlotsBegin,
        InstancePrimVar  // has to be at the very end
    };

    enum DirtyBits : HdDirtyBits {
        DirtyIndices        = HdChangeTracker::CustomBitsBegin,
        DirtyHullIndices    = (DirtyIndices       << 1)
    };

    // When processing primvars, these will get set to if we determine that
    // we should do cubic basis interpolation on the normals and widths.
    // NOTE: I worry that it may be possible for these to get out of sync.
    // The right long term fix is likely to maintain proper separation between 
    // varying and vertex primvars throughout the HdSt rendering pipeline.
    bool _basisWidthInterpolation = false;
    bool _basisNormalInterpolation = false;

    bool _SupportsRefinement(int refineLevel);
    bool _SupportsUserWidths(HdStDrawItem* drawItem);
    bool _SupportsUserNormals(HdStDrawItem* drawItem);

    void _UpdateDrawItem(HdSceneDelegate *sceneDelegate,
                         HdStDrawItem *drawItem,
                         HdDirtyBits *dirtyBits,
                         const HdBasisCurvesReprDesc &desc);

    void _UpdateDrawItemGeometricShader(HdSceneDelegate *sceneDelegate,
                                        HdStDrawItem *drawItem,
                                        const HdBasisCurvesReprDesc &desc);

    HdSt_BasisCurvesTopologySharedPtr _topology;
    HdTopology::ID _topologyId;
    HdDirtyBits _customDirtyBitsInUse;
    int _refineLevel;  // XXX: could be moved into HdBasisCurveTopology.
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // HDST_BASIS_CURVES_H
