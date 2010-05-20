/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include "CudaCommon.h"
#include "CudaMath.h"
#include "CudaTexture.h"
#include "cuda.h"
#include "mycuda.h"

//#define umul24(x,y) ((x)*(y))

#if defined(__cplusplus)
namespace sofa
{
namespace gpu
{
namespace cuda
{
#endif

extern "C"
{
    void TetrahedronFEMForceFieldCuda3f_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3f_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3f_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);


    void TetrahedronFEMForceFieldCuda3f1_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3f1_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3f1_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);

    void TetrahedronFEMForceFieldCuda3f_getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations);

    struct TetraFEMForceOp
    {
        unsigned int nbElem;
        unsigned int nbVertex;
        unsigned int nbElemPerVertex;
        const void* elems;
        void* rotations;
        void* eforce;
        const void* velems;
        void* f;
        const void* x;
        const void* v;
    };

    struct TetraFEMDForceOp
    {
        unsigned int nbElem;
        unsigned int nbVertex;
        unsigned int nbElemPerVertex;
        const void* elems;
        const void* rotations;
        void* eforce;
        const void* velems;
        void* df;
        const void* dx;
    };

    void MultiTetrahedronFEMForceFieldCuda3f_addForce(int n, TetraFEMForceOp* ops);
    void MultiTetrahedronFEMForceFieldCuda3f_addDForce(int n, TetraFEMDForceOp* ops, double factor);

#ifdef SOFA_GPU_CUDA_DOUBLE

    void TetrahedronFEMForceFieldCuda3d_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3d_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3d_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);

    void TetrahedronFEMForceFieldCuda3d1_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v);
    void TetrahedronFEMForceFieldCuda3d1_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor);
    void TetrahedronFEMForceFieldCuda3d1_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor);

    void TetrahedronFEMForceFieldCuda3d_getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations);

#endif // SOFA_GPU_CUDA_DOUBLE
}

template<class real>
class __align__(16) GPUElement
{
public:
    /// index of the 4 connected vertices
    //Vec<4,int> tetra;
    int ia[BSIZE];
    int ib[BSIZE];
    int ic[BSIZE];
    int id[BSIZE];
    /// material stiffness matrix
    //Mat<6,6,Real> K;
    real gamma_bx2[BSIZE], mu2_bx2[BSIZE];
    /// initial position of the vertices in the local (rotated) coordinate system
    //Vec3f initpos[4];
    real bx[BSIZE],cx[BSIZE];
    real cy[BSIZE],dx[BSIZE],dy[BSIZE],dz[BSIZE];
    /// strain-displacement matrix
    //Mat<12,6,Real> J;
    real Jbx_bx[BSIZE],Jby_bx[BSIZE],Jbz_bx[BSIZE];
    /// unused value to align to 64 bytes
    //real dummy[BSIZE];
};

template<class real>
class GPUElementForce
{
public:
    CudaVec4<real> fA,fB,fC,fD;
};

//////////////////////
// GPU-side methods //
//////////////////////

//#define USE_TEXTURE_X false
//#define USE_TEXTURE_ELEMENT_FORCE false

/*
template<typename real, class TIn>
class CudaTetrahedronFEMForceFieldInputTextures
{
    static InputVector<TIn, CudaVec3<real>, USE_TEXTURE_X>& X()
    {
	static InputVector<TIn, CudaVec3<real>, USE_TEXTURE_X> v; return v;
    }
    static InputVector<TIn, CudaVec3<real>, USE_TEXTURE_X>& DX()
    {
	static InputVector<TIn, CudaVec3<real>, USE_TEXTURE_X> v; return v;
    }

public:

    static __host__ void setX(const void* x)
    {
	X().set((const TIn*)x);
    }

    static __inline__ __device__ CudaVec3<real> getX(int i, const TIn* x)
    {
	return X().get(i, (const TIn*)x);
    }

    static __host__ void setDX(const void* x)
    {
	DX().set((const TIn*)x);
    }

    static __inline__ __device__ CudaVec3<real> getDX(int i, const TIn* x)
    {
	return DX().get(i, (const TIn*)x);
    }
};

template<typename real, class TIn>
class CudaTetrahedronFEMForceFieldTempTextures
{
    static InputVector<TIn, CudaVec3<real>, USE_TEXTURE_ELEMENT_FORCE>& ElementForce();
    {
	static InputVector<TIn, CudaVec3<real>, USE_TEXTURE_ELEMENT_FORCE> v; return v;
    }

public:

    static __host__ void setElementForce(const void* x)
    {
	ElementForce().set((const TIn*)x);
    }

    static __inline__ __device__ CudaVec3<real> getElementForce(int i, const TIn* x)
    {
	return ElementForce().get(i, (const TIn*)x);
    }

};
*/

// no texture is used unless this template is specialized
template<typename real, class TIn>
class CudaTetrahedronFEMForceFieldInputTextures
{
public:

    static __host__ void setX(const void* /*x*/)
    {
    }

    static __inline__ __device__ CudaVec3<real> getX(int i, const TIn* x)
    {
        return CudaVec3<real>::make(x[i]);
    }

    static __host__ void setDX(const void* /*x*/)
    {
    }

    static __inline__ __device__ CudaVec3<real> getDX(int i, const TIn* x)
    {
        return CudaVec3<real>::make(x[i]);
    }
};


// no texture is used unless this template is specialized
template<typename real, class TIn>
class CudaTetrahedronFEMForceFieldTempTextures
{
public:

    static __host__ void setElementForce(const void* /*x*/)
    {
    }

    static __inline__ __device__ CudaVec3<real> getElementForce(int i, const TIn* x)
    {
        return CudaVec3<real>::make(x[i]);
    }
};

#if defined(USE_TEXTURE_X)

static texture<float,1,cudaReadModeElementType> tex_3f_x;
static texture<float,1,cudaReadModeElementType> tex_3f_dx;

template<>
class CudaTetrahedronFEMForceFieldInputTextures<float, CudaVec3<float> >
{
public:
    typedef float real;
    typedef CudaVec3<real> TIn;

    static __host__ void setX(const void* x)
    {
        static const void* cur = NULL;
        if (x!=cur)
        {
            cudaBindTexture((size_t*)NULL, tex_3f_x, x);
            cur = x;
        }
    }

    static __inline__ __device__ CudaVec3<real> getX(int i, const TIn* x)
    {
        int i3 = umul24(i,3);
        float x1 = tex1Dfetch(tex_3f_x, i3);
        float x2 = tex1Dfetch(tex_3f_x, i3+1);
        float x3 = tex1Dfetch(tex_3f_x, i3+2);
        return CudaVec3<real>::make(x1,x2,x3);
    }

    static __host__ void setDX(const void* dx)
    {
        static const void* cur = NULL;
        if (dx!=cur)
        {
            cudaBindTexture((size_t*)NULL, tex_3f_dx, dx);
            cur = dx;
        }
    }

    static __inline__ __device__ CudaVec3<real> getDX(int i, const TIn* dx)
    {
        int i3 = umul24(i,3);
        float x1 = tex1Dfetch(tex_3f_dx, i3);
        float x2 = tex1Dfetch(tex_3f_dx, i3+1);
        float x3 = tex1Dfetch(tex_3f_dx, i3+2);
        return CudaVec3<real>::make(x1,x2,x3);
    }
};

static texture<float4,1,cudaReadModeElementType> tex_3f1_x;
static texture<float4,1,cudaReadModeElementType> tex_3f1_dx;

template<>
class CudaTetrahedronFEMForceFieldInputTextures<float, CudaVec4<float> >
{
public:
    typedef float real;
    typedef CudaVec4<real> TIn;

    static __host__ void setX(const void* x)
    {
        static const void* cur = NULL;
        if (x!=cur)
        {
            cudaBindTexture((size_t*)NULL, tex_3f1_x, x);
            cur = x;
        }
    }

    static __inline__ __device__ CudaVec3<real> getX(int i, const TIn* x)
    {
        return CudaVec3<real>::make(tex1Dfetch(tex_3f1_x, i));
    }

    static __host__ void setDX(const void* dx)
    {
        static const void* cur = NULL;
        if (dx!=cur)
        {
            cudaBindTexture((size_t*)NULL, tex_3f1_dx, dx);
            cur = dx;
        }
    }

    static __inline__ __device__ CudaVec3<real> getDX(int i, const TIn* dx)
    {
        return CudaVec3<real>::make(tex1Dfetch(tex_3f1_dx, i));
    }
};

#endif


#if defined(USE_TEXTURE_ELEMENT_FORCE)

static texture<float,1,cudaReadModeElementType> tex_3f_eforce;

template<>
class CudaTetrahedronFEMForceFieldTempTextures<float, CudaVec3<float> >
{
public:
    typedef float real;
    typedef CudaVec3<real> TIn;

    static __host__ void setElementForce(const void* x)
    {
        static const void* cur = NULL;
        if (x!=cur)
        {
            cudaBindTexture((size_t*)NULL, tex_3f_eforce, x);
            cur = x;
        }
    }

    static __inline__ __device__ CudaVec3<real> getElementForce(int i, const TIn* x)
    {
        int i3 = umul24(i,3);
        float x1 = tex1Dfetch(tex_3f_eforce, i3);
        float x2 = tex1Dfetch(tex_3f_eforce, i3+1);
        float x3 = tex1Dfetch(tex_3f_eforce, i3+2);
        return CudaVec3<real>::make(x1,x2,x3);
    }
};

static texture<float4,1,cudaReadModeElementType> tex_3f1_eforce;

template<>
class CudaTetrahedronFEMForceFieldTempTextures<float, CudaVec4<float> >
{
public:
    typedef float real;
    typedef CudaVec4<real> TIn;
    typedef CudaVec3<real> TOut;

    static __host__ void setElementForce(const void* x)
    {
        static const void* cur = NULL;
        if (x!=cur)
        {
            cudaBindTexture((size_t*)NULL, tex_3f1_eforce, x);
            cur = x;
        }
    }

    static __inline__ __device__ CudaVec3<real> getElementForce(int i, const TIn* x)
    {
        return CudaVec3<real>::make(tex1Dfetch(tex_3f1_eforce, i));
    }
};

#endif


template<typename real, class TIn>
__global__ void TetrahedronFEMForceFieldCuda3t_calcForce_kernel(int nbElem, const GPUElement<real>* elems, real* rotations, real* eforce, const TIn* x)
{
    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    int index1 = threadIdx.x;
    int index = index0+index1;

    //GPUElement<real> e = elems[index];
    //GPUElementState<real> s;
    const GPUElement<real>* e = elems + blockIdx.x;
    matrix3<real> Rt;
    rotations += umul24(index0,9)+index1;
    //GPUElementForce<real> f;
    CudaVec3<real> fB,fC,fD;

    if (index < nbElem)
    {
        CudaVec3<real> A = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->ia[index1], x); //((const CudaVec3<real>*)x)[e.ia];
        CudaVec3<real> B = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->ib[index1], x); //((const CudaVec3<real>*)x)[e.ib];
        B -= A;

        // Compute R
        real bx = norm(B);
        Rt.x = B/bx;
        // Compute JtRtX = JbtRtB + JctRtC + JdtRtD

        CudaVec3<real> JtRtX0,JtRtX1;

        bx -= e->bx[index1];
        //                    ( bx)
        // RtB =              ( 0 )
        //                    ( 0 )
        // Jtb = (Jbx  0   0 )
        //       ( 0  Jby  0 )
        //       ( 0   0  Jbz)
        //       (Jby Jbx  0 )
        //       ( 0  Jbz Jby)
        //       (Jbz  0  Jbx)
        real e_Jbx_bx = e->Jbx_bx[index1];
        real e_Jby_bx = e->Jby_bx[index1];
        real e_Jbz_bx = e->Jbz_bx[index1];
        JtRtX0.x = e_Jbx_bx * bx;
        JtRtX0.y = 0;
        JtRtX0.z = 0;
        JtRtX1.x = e_Jby_bx * bx;
        JtRtX1.y = 0;
        JtRtX1.z = e_Jbz_bx * bx;

        CudaVec3<real> C = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->ic[index1], x); //((const CudaVec3<real>*)x)[e.ic];
        C -= A;
        Rt.z = cross(B,C);
        Rt.y = cross(Rt.z,B);
        Rt.y *= invnorm(Rt.y);
        Rt.z *= invnorm(Rt.z);

        real e_cx = e->cx[index1];
        real e_cy = e->cy[index1];
        real cx = Rt.mulX(C) - e_cx;
        real cy = Rt.mulY(C) - e_cy;
        //                    ( cx)
        // RtC =              ( cy)
        //                    ( 0 )
        // Jtc = ( 0   0   0 )
        //       ( 0   dz  0 )
        //       ( 0   0  -dy)
        //       ( dz  0   0 )
        //       ( 0  -dy  dz)
        //       (-dy  0   0 )
        real e_dy = e->dy[index1];
        real e_dz = e->dz[index1];
        //JtRtX0.x += 0;
        JtRtX0.y += e_dz * cy;
        //JtRtX0.z += 0;
        JtRtX1.x += e_dz * cx;
        JtRtX1.y -= e_dy * cy;
        JtRtX1.z -= e_dy * cx;

        CudaVec3<real> D = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->id[index1], x); //((const CudaVec3<real>*)x)[e.id];
        D -= A;

        real e_dx = e->dx[index1];
        real dx = Rt.mulX(D) - e_dx;
        real dy = Rt.mulY(D) - e_dy;
        real dz = Rt.mulZ(D) - e_dz;
        //                    ( dx)
        // RtD =              ( dy)
        //                    ( dz)
        // Jtd = ( 0   0   0 )
        //       ( 0   0   0 )
        //       ( 0   0   cy)
        //       ( 0   0   0 )
        //       ( 0   cy  0 )
        //       ( cy  0   0 )
        //JtRtX0.x += 0;
        //JtRtX0.y += 0;
        JtRtX0.z += e_cy * dz;
        //JtRtX1.x += 0;
        JtRtX1.y += e_cy * dy;
        JtRtX1.z += e_cy * dx;

        // Compute S = K JtRtX

        // K = [ gamma+mu2 gamma gamma 0 0 0 ]
        //     [ gamma gamma+mu2 gamma 0 0 0 ]
        //     [ gamma gamma gamma+mu2 0 0 0 ]
        //     [ 0 0 0             mu2/2 0 0 ]
        //     [ 0 0 0             0 mu2/2 0 ]
        //     [ 0 0 0             0 0 mu2/2 ]
        // S0 = JtRtX0*mu2 + dot(JtRtX0,(gamma gamma gamma))
        // S1 = JtRtX1*mu2/2

        real e_mu2_bx2 = e->mu2_bx2[index1];
        CudaVec3<real> S0  = JtRtX0*e_mu2_bx2;
        S0 += (JtRtX0.x+JtRtX0.y+JtRtX0.z)*e->gamma_bx2[index1];
        CudaVec3<real> S1  = JtRtX1*(e_mu2_bx2*0.5f);

        // Jd = ( 0   0   0   0   0  cy )
        //      ( 0   0   0   0  cy   0 )
        //      ( 0   0   cy  0   0   0 )
        fD = (Rt.mulT(CudaVec3<real>::make(
                e_cy * S1.z,
                e_cy * S1.y,
                e_cy * S0.z)));
        // Jc = ( 0   0   0  dz   0 -dy )
        //      ( 0   dz  0   0 -dy   0 )
        //      ( 0   0  -dy  0  dz   0 )
        fC = (Rt.mulT(CudaVec3<real>::make(
                e_dz * S1.x - e_dy * S1.z,
                e_dz * S0.y - e_dy * S1.y,
                e_dz * S1.y - e_dy * S0.z)));
        // Jb = (Jbx  0   0  Jby  0  Jbz)
        //      ( 0  Jby  0  Jbx Jbz  0 )
        //      ( 0   0  Jbz  0  Jby Jbx)
        fB = (Rt.mulT(CudaVec3<real>::make(
                e_Jbx_bx * S0.x                                     + e_Jby_bx * S1.x                   + e_Jbz_bx * S1.z,
                e_Jby_bx * S0.y                   + e_Jbx_bx * S1.x + e_Jbz_bx * S1.y,
                e_Jbz_bx * S0.z                   + e_Jby_bx * S1.y + e_Jbx_bx * S1.z)));
        //fA.x = -(fB.x+fC.x+fD.x);
        //fA.y = -(fB.y+fC.y+fD.y);
        //fA.z = -(fB.z+fC.z+fD.z);
    }

    //state[index] = s;
    Rt.writeAoS(rotations);
    //((rmatrix3*)rotations)[index] = Rt;
    //((GPUElementForce<real>*)eforce)[index] = f;

    //! Dynamically allocated shared memory to reorder global memory access
    __shared__  real temp[BSIZE*13];
    int index13 = umul24(index1,13);
    temp[index13+0 ] = -(fB.x+fC.x+fD.x);
    temp[index13+1 ] = -(fB.y+fC.y+fD.y);
    temp[index13+2 ] = -(fB.z+fC.z+fD.z);
    temp[index13+3 ] = fB.x;
    temp[index13+4 ] = fB.y;
    temp[index13+5 ] = fB.z;
    temp[index13+6 ] = fC.x;
    temp[index13+7 ] = fC.y;
    temp[index13+8 ] = fC.z;
    temp[index13+9 ] = fD.x;
    temp[index13+10] = fD.y;
    temp[index13+11] = fD.z;
    __syncthreads();
    real* out = ((real*)eforce)+(umul24(blockIdx.x,BSIZE*16))+index1;
    real v = 0;
    bool read = true; //(index1&4)<3;
    index1 += (index1>>4) - (index1>>2); // remove one for each 4-values before this thread, but add an extra one each 16 threads (so each 12 input cells, to align to 13)

    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;

}

template<typename real>
__global__ void TetrahedronFEMForceFieldCuda3t_addForce_kernel(int nbVertex, unsigned int nbElemPerVertex, const CudaVec4<real>* eforce, const int* velems, real* f)
{
    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    int index1 = threadIdx.x;
    int index3 = umul24(index1,3); //3*index1;

    //! Dynamically allocated shared memory to reorder global memory access
    __shared__  real temp[BSIZE*3];

    // First copy x inside temp
    int iext = umul24(blockIdx.x,BSIZE*3)+index1; //index0*3+index1;

    CudaVec3<real> force = CudaVec3<real>::make(0.0f,0.0f,0.0f);

    velems+=umul24(index0,nbElemPerVertex)+index1;

    if (index0+index1 < nbVertex)
        for (int s = 0; s < nbElemPerVertex; s++)
        {
            int i = *velems -1;
            velems+=BSIZE;
            if (i != -1)
            {
                force -= CudaTetrahedronFEMForceFieldTempTextures<real,CudaVec4<real> >::getElementForce(i,eforce);
            }
        }

    temp[index3  ] = force.x;
    temp[index3+1] = force.y;
    temp[index3+2] = force.z;

    __syncthreads();

    f[iext        ] += temp[index1        ];
    f[iext+  BSIZE] += temp[index1+  BSIZE];
    f[iext+2*BSIZE] += temp[index1+2*BSIZE];
}

template<typename real>
__global__ void TetrahedronFEMForceFieldCuda3t1_addForce_kernel(int nbVertex, unsigned int nbElemPerVertex, const CudaVec4<real>* eforce, const int* velems, CudaVec4<real>* f)
{
    const int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    const int index1 = threadIdx.x;
    const int index = index0 + index1;

    CudaVec3<real> force = CudaVec3<real>::make(0.0f,0.0f,0.0f);

    velems+=umul24(index0,nbElemPerVertex)+index1;

    if (index < nbVertex)
        for (int s = 0; s < nbElemPerVertex; s++)
        {
            int i = *velems -1;
            velems+=BSIZE;
            if (i != -1)
            {
                force -= CudaTetrahedronFEMForceFieldTempTextures<real,CudaVec4<real> >::getElementForce(i,eforce);
            }
        }
    CudaVec4<real> fi = f[index];
    fi.x += force.x;
    fi.y += force.y;
    fi.z += force.z;
    f[index] = fi;
}

template<typename real, class TIn>
__global__ void TetrahedronFEMForceFieldCuda3t_calcDForce_kernel(int nbElem, const GPUElement<real>* elems, const real* rotations, real* eforce, const TIn* x, real factor)
{
    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    int index1 = threadIdx.x;
    int index = index0+index1;

    //GPUElement<real> e = elems[index];
    const GPUElement<real>* e = elems + blockIdx.x;
    //GPUElementState<real> s = state[index];
    //GPUElementForce<real> f;
    CudaVec3<real> fB,fC,fD;
    matrix3<real> Rt;
    rotations += umul24(index0,9)+index1;
    Rt.readAoS(rotations);
    //Rt = ((const rmatrix3*)rotations)[index];

    if (index < nbElem)
    {
        // Compute JtRtX = JbtRtB + JctRtC + JdtRtD

        CudaVec3<real> A = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->ia[index1], x); //((const CudaVec3<real>*)x)[e.ia];
        CudaVec3<real> JtRtX0,JtRtX1;


        CudaVec3<real> B = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->ib[index1], x); //((const CudaVec3<real>*)x)[e.ib];
        B = Rt * (B-A);

        // Jtb = (Jbx  0   0 )
        //       ( 0  Jby  0 )
        //       ( 0   0  Jbz)
        //       (Jby Jbx  0 )
        //       ( 0  Jbz Jby)
        //       (Jbz  0  Jbx)
        real e_Jbx_bx = e->Jbx_bx[index1];
        real e_Jby_bx = e->Jby_bx[index1];
        real e_Jbz_bx = e->Jbz_bx[index1];
        JtRtX0.x = e_Jbx_bx * B.x;
        JtRtX0.y =                  e_Jby_bx * B.y;
        JtRtX0.z =                                   e_Jbz_bx * B.z;
        JtRtX1.x = e_Jby_bx * B.x + e_Jbx_bx * B.y;
        JtRtX1.y =                  e_Jbz_bx * B.y + e_Jby_bx * B.z;
        JtRtX1.z = e_Jbz_bx * B.x                  + e_Jbx_bx * B.z;

        CudaVec3<real> C = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->ic[index1], x); //((const CudaVec3<real>*)x)[e.ic];
        C = Rt * (C-A);

        // Jtc = ( 0   0   0 )
        //       ( 0   dz  0 )
        //       ( 0   0  -dy)
        //       ( dz  0   0 )
        //       ( 0  -dy  dz)
        //       (-dy  0   0 )
        real e_dy = e->dy[index1];
        real e_dz = e->dz[index1];
        //JtRtX0.x += 0;
        JtRtX0.y +=              e_dz * C.y;
        JtRtX0.z +=                         - e_dy * C.z;
        JtRtX1.x += e_dz * C.x;
        JtRtX1.y +=            - e_dy * C.y + e_dz * C.z;
        JtRtX1.z -= e_dy * C.x;

        // Jtd = ( 0   0   0 )
        //       ( 0   0   0 )
        //       ( 0   0   cy)
        //       ( 0   0   0 )
        //       ( 0   cy  0 )
        //       ( cy  0   0 )
        CudaVec3<real> D = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->id[index1], x); //((const CudaVec3<real>*)x)[e.id];
        D = Rt * (D-A);

        real e_cy = e->cy[index1];
        //JtRtX0.x += 0;
        //JtRtX0.y += 0;
        JtRtX0.z +=                           e_cy * D.z;
        //JtRtX1.x += 0;
        JtRtX1.y +=              e_cy * D.y;
        JtRtX1.z += e_cy * D.x;

        // Compute S = K JtRtX

        // K = [ gamma+mu2 gamma gamma 0 0 0 ]
        //     [ gamma gamma+mu2 gamma 0 0 0 ]
        //     [ gamma gamma gamma+mu2 0 0 0 ]
        //     [ 0 0 0             mu2/2 0 0 ]
        //     [ 0 0 0             0 mu2/2 0 ]
        //     [ 0 0 0             0 0 mu2/2 ]
        // S0 = JtRtX0*mu2 + dot(JtRtX0,(gamma gamma gamma))
        // S1 = JtRtX1*mu2/2

        real e_mu2_bx2 = e->mu2_bx2[index1];
        CudaVec3<real> S0  = JtRtX0*e_mu2_bx2;
        S0 += (JtRtX0.x+JtRtX0.y+JtRtX0.z)*e->gamma_bx2[index1];
        CudaVec3<real> S1  = JtRtX1*(e_mu2_bx2*0.5f);

        S0 *= factor;
        S1 *= factor;

        // Jd = ( 0   0   0   0   0  cy )
        //      ( 0   0   0   0  cy   0 )
        //      ( 0   0   cy  0   0   0 )
        fD = (Rt.mulT(CudaVec3<real>::make(
                e_cy * S1.z,
                e_cy * S1.y,
                e_cy * S0.z)));
        // Jc = ( 0   0   0  dz   0 -dy )
        //      ( 0   dz  0   0 -dy   0 )
        //      ( 0   0  -dy  0  dz   0 )
        fC = (Rt.mulT(CudaVec3<real>::make(
                e_dz * S1.x - e_dy * S1.z,
                e_dz * S0.y - e_dy * S1.y,
                e_dz * S1.y - e_dy * S0.z)));
        // Jb = (Jbx  0   0  Jby  0  Jbz)
        //      ( 0  Jby  0  Jbx Jbz  0 )
        //      ( 0   0  Jbz  0  Jby Jbx)
        fB = (Rt.mulT(CudaVec3<real>::make(
                e_Jbx_bx * S0.x                                     + e_Jby_bx * S1.x                   + e_Jbz_bx * S1.z,
                e_Jby_bx * S0.y                   + e_Jbx_bx * S1.x + e_Jbz_bx * S1.y,
                e_Jbz_bx * S0.z                   + e_Jby_bx * S1.y + e_Jbx_bx * S1.z)));
        //fA.x = -(fB.x+fC.x+fD.x);
        //fA.y = -(fB.y+fC.y+fD.y);
        //fA.z = -(fB.z+fC.z+fD.z);
    }

    //state[index] = s;
    //((GPUElementForce<real>*)eforce)[index] = f;

    //! Dynamically allocated shared memory to reorder global memory access
    __shared__  real temp[BSIZE*13];
    int index13 = umul24(index1,13);
    temp[index13+0 ] = -(fB.x+fC.x+fD.x);
    temp[index13+1 ] = -(fB.y+fC.y+fD.y);
    temp[index13+2 ] = -(fB.z+fC.z+fD.z);
    temp[index13+3 ] = fB.x;
    temp[index13+4 ] = fB.y;
    temp[index13+5 ] = fB.z;
    temp[index13+6 ] = fC.x;
    temp[index13+7 ] = fC.y;
    temp[index13+8 ] = fC.z;
    temp[index13+9 ] = fD.x;
    temp[index13+10] = fD.y;
    temp[index13+11] = fD.z;
    __syncthreads();
    real* out = ((real*)eforce)+(umul24(blockIdx.x,BSIZE*16))+index1;
    real v = 0;
    bool read = true; //(index1&4)<3;
    index1 += (index1>>4) - (index1>>2); // remove one for each 4-values before this thread, but add an extra one each 16 threads (so each 12 input cells, to align to 13)

    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
}

template<typename real, class TIn>
__global__ void TetrahedronFEMForceFieldCuda3t_addKToMatrix_kernel(int nbElem, const GPUElement<real>* elems, const real* rotations, real* eforce, real factor)
{
    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    int index1 = threadIdx.x;
    int index = index0+index1;

    const GPUElement<real>* e = elems + blockIdx.x;
    matrix3<real> Rt;
    rotations += umul24(index0,9)+index1;
    Rt.readAoS(rotations);

    __shared__  real temp[BSIZE*13];
    int index13 = umul24(index1,13);

    if (index < nbElem)
    {
        matrix3<real> M;
        CudaVec3<real> T1; //one line of Jx * K

        real bx = e->Jbx_bx[index1];
        real by = e->Jby_bx[index1];
        real bz = e->Jbz_bx[index1];
        real dy = e->dy[index1];
        real dz = e->dz[index1];
        real cy = e->cy[index1];
        real ax = -bx;
        real ay = -by-dz;
        real az = dy-bz-cy;


        // K = [ gamma+mu2 gamma     gamma       0    0      0 ]
        //     [ gamma     gamma+mu2 gamma       0    0      0 ]
        //     [ gamma     gamma     gamma+mu2   0    0      0 ]
        //     [ 0           0         0       mu2/2  0      0 ]
        //     [ 0           0         0         0   mu2/2   0 ]
        //     [ 0           0         0         0    0    mu2/2 ]
        real gam = e->gamma_bx2[index1] * factor;
        real mu2 = e->mu2_bx2[index1] * factor;
        real gpm = gam + mu2;
        real md2 = mu2*0.5f;

        //Compute M==Ja K Jat

        // Ja = (-bx   0      0     -by-dz     0       dy-bz-cy)
        //      ( 0  -by-dz   0      -bx   dy-cy-bz       0    )
        //      ( 0     0   dy-cy-bz   0      -by-dz     -bx   )
        T1.x = ax*ax; T1.y = ay*ay; T1.z = az*az;
        M.x.x = T1.x * gpm + (T1.y + T1.z)*md2; M.x.y = (gam + md2) * ax * az;          M.x.z = (gam + md2) * ax * az;
        M.y.x = M.x.y;				M.y.y = T1.y * gpm + (T1.x + T1.z)*md2; M.y.z = (gam + md2) * ay * az;
        M.z.x = M.x.z;				M.z.y = M.y.z;			        M.z.z = T1.z * gpm + (T1.x + T1.y) * md2;

        M = Rt.mulT(M) * Rt;

        temp[index13+0 ] = M.x.x;
        temp[index13+1 ] = M.y.y;
        temp[index13+2 ] = M.z.z;

        //Compute Maa==Jb K Jbt

        // Jb = (bx  0   0  by  0 bz)
        //      (0  by   0  bx bz  0)
        //      (0   0  bz  0  by bx)
        T1.x = bx*bx; T1.y = by*by; T1.z = bz*bz;
        M.x.x = T1.x * gpm + (T1.y + T1.z)*md2; M.x.y = (gam + md2) * bx * bz;          M.x.z = (gam + md2) * bx * bz;
        M.y.x = M.x.y;				M.y.y = T1.y * gpm + (T1.x + T1.z)*md2; M.y.z = (gam + md2) * by * bz;
        M.z.x = M.x.z;				M.z.y = M.y.z;				M.z.z = T1.z * gpm + (T1.x + T1.y) * md2;

        M = Rt.mulT(M) * Rt;

        temp[index13+3 ] = M.x.x;
        temp[index13+4 ] = M.y.y;
        temp[index13+5 ] = M.z.z;

        //Compute Maa==Jc K Jct

        // Jc = ( 0   0   0  dz   0 -dy )
        //      ( 0   dz  0   0 -dy   0 )
        //      ( 0   0  -dy  0  dz   0 )
        T1.x = dz*dz;
        T1.y = (-dy)*(-dy);
        M.x.x = (T1.x+T1.y)*md2; M.x.y = 0;                   M.x.z = 0;
        M.y.x = 0;               M.y.y = T1.x*gpm + T1.y*md2; M.y.z = (gam+md2)*dz*(-dy);
        M.z.x = 0;               M.z.y = M.y.z;               M.z.z = T1.y*gpm + T1.x*md2;

        M = Rt.mulT(M) * Rt;

        temp[index13+6 ] = M.x.x;
        temp[index13+7 ] = M.y.y;
        temp[index13+8 ] = M.z.z;

        //Compute Maa==Jd K Jdt

        // Jd = ( 0   0   0   0   0  cy )
        //      ( 0   0   0   0  cy   0 )
        //      ( 0   0   cy  0   0   0 )
        M.x.x =  cy*cy*md2; M.x.y =  0;	    M.x.z =  0;
        M.y.x =  0;         M.y.y =  M.x.x; M.y.z =  0;
        M.z.x =  0;         M.z.y =  0;     M.z.z =  cy*cy*gpm;

        M = Rt.mulT(M) * Rt;

        temp[index13+9 ] = M.x.x;
        temp[index13+10] = M.y.y;
        temp[index13+11] = M.z.z;
    }
    __syncthreads();
    real* out = ((real*)eforce)+(umul24(blockIdx.x,BSIZE*16))+index1;
    real v = 0;
    bool read = true; //(index1&4)<3;
    index1 += (index1>>4) - (index1>>2); // remove one for each 4-values before this thread, but add an extra one each 16 threads (so each 12 input cells, to align to 13)

    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
}

template<typename real>
__global__ void TetrahedronFEMForceFieldCuda3t_getRotations_kernel(int nbVertex, const real* initState, const real* state, const int* rotationIdx, real* rotations)
{
    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    int index1 = threadIdx.x;
    int index = index0+index1;

    if (index>=nbVertex) return;

    const int elemIdx = rotationIdx[index];
    const int stateIdx = ((elemIdx / BSIZE) * (9*BSIZE)) + (elemIdx % BSIZE);

    matrix3<real> initRt, curRt, R;

    initRt.readAoS(initState + stateIdx);
    curRt.readAoS(state + stateIdx);

    // R = transpose(curRt) * initRt
    R = curRt.mulT(initRt);

    //R.x.x = dot (curR.x, initR.x); R.x.y = dot (curR.x, initR.y); R.x.z = dot (curR.x, initR.z);
    //R.y.x = dot (curR.y, initR.x); R.y.y = dot (curR.y, initR.y); R.y.z = dot (curR.y, initR.z);
    //R.z.x = dot (curR.z, initR.x); R.z.y = dot (curR.z, initR.y); R.z.z = dot (curR.z, initR.z);

    rotations += 9*index;
    rotations[0] = R.x.x;
    rotations[1] = R.x.y;
    rotations[2] = R.x.z;

    rotations[3] = R.y.x;
    rotations[4] = R.y.y;
    rotations[5] = R.y.z;

    rotations[6] = R.z.x;
    rotations[7] = R.z.y;
    rotations[8] = R.z.z;
}

////////////////////////
// GPU merged methods //
////////////////////////

enum { MULTI_NMAX = 64 };
__constant__ TetraFEMForceOp multiTetraFEMForceOps[MULTI_NMAX];
__constant__ TetraFEMDForceOp multiTetraFEMDForceOps[MULTI_NMAX];

template<typename real, class TIn>
__global__ void MultiTetrahedronFEMForceFieldCuda3t_calcForce_kernel()
{
    int nbElem = multiTetraFEMForceOps[blockIdx.y].nbElem;
    const GPUElement<real>* elems = (const GPUElement<real>*) multiTetraFEMForceOps[blockIdx.y].elems;
    real* rotations = (real*) multiTetraFEMForceOps[blockIdx.y].rotations;
    real* eforce = (real*) multiTetraFEMForceOps[blockIdx.y].eforce;
    const TIn* x = (const TIn*) multiTetraFEMForceOps[blockIdx.y].x;

    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    if (index0 >= nbElem) return;
    int index1 = threadIdx.x;
    int index = index0+index1;

    //GPUElement<real> e = elems[index];
    //GPUElementState<real> s;
    const GPUElement<real>* e = elems + blockIdx.x;
    matrix3<real> Rt;
    rotations += umul24(index0,9)+index1;
    //GPUElementForce<real> f;
    CudaVec3<real> fB,fC,fD;

    if (index < nbElem)
    {
        CudaVec3<real> A = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->ia[index1], x); //((const CudaVec3<real>*)x)[e.ia];
        CudaVec3<real> B = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->ib[index1], x); //((const CudaVec3<real>*)x)[e.ib];
        B -= A;

        // Compute R
        real bx = norm(B);
        Rt.x = B/bx;
        // Compute JtRtX = JbtRtB + JctRtC + JdtRtD

        CudaVec3<real> JtRtX0,JtRtX1;

        bx -= e->bx[index1];
        //                    ( bx)
        // RtB =              ( 0 )
        //                    ( 0 )
        // Jtb = (Jbx  0   0 )
        //       ( 0  Jby  0 )
        //       ( 0   0  Jbz)
        //       (Jby Jbx  0 )
        //       ( 0  Jbz Jby)
        //       (Jbz  0  Jbx)
        real e_Jbx_bx = e->Jbx_bx[index1];
        real e_Jby_bx = e->Jby_bx[index1];
        real e_Jbz_bx = e->Jbz_bx[index1];
        JtRtX0.x = e_Jbx_bx * bx;
        JtRtX0.y = 0;
        JtRtX0.z = 0;
        JtRtX1.x = e_Jby_bx * bx;
        JtRtX1.y = 0;
        JtRtX1.z = e_Jbz_bx * bx;

        CudaVec3<real> C = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->ic[index1], x); //((const CudaVec3<real>*)x)[e.ic];
        C -= A;
        Rt.z = cross(B,C);
        Rt.y = cross(Rt.z,B);
        Rt.y *= invnorm(Rt.y);
        Rt.z *= invnorm(Rt.z);

        real e_cx = e->cx[index1];
        real e_cy = e->cy[index1];
        real cx = Rt.mulX(C) - e_cx;
        real cy = Rt.mulY(C) - e_cy;
        //                    ( cx)
        // RtC =              ( cy)
        //                    ( 0 )
        // Jtc = ( 0   0   0 )
        //       ( 0   dz  0 )
        //       ( 0   0  -dy)
        //       ( dz  0   0 )
        //       ( 0  -dy  dz)
        //       (-dy  0   0 )
        real e_dy = e->dy[index1];
        real e_dz = e->dz[index1];
        //JtRtX0.x += 0;
        JtRtX0.y += e_dz * cy;
        //JtRtX0.z += 0;
        JtRtX1.x += e_dz * cx;
        JtRtX1.y -= e_dy * cy;
        JtRtX1.z -= e_dy * cx;

        CudaVec3<real> D = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getX(e->id[index1], x); //((const CudaVec3<real>*)x)[e.id];
        D -= A;

        real e_dx = e->dx[index1];
        real dx = Rt.mulX(D) - e_dx;
        real dy = Rt.mulY(D) - e_dy;
        real dz = Rt.mulZ(D) - e_dz;
        //                    ( dx)
        // RtD =              ( dy)
        //                    ( dz)
        // Jtd = ( 0   0   0 )
        //       ( 0   0   0 )
        //       ( 0   0   cy)
        //       ( 0   0   0 )
        //       ( 0   cy  0 )
        //       ( cy  0   0 )
        //JtRtX0.x += 0;
        //JtRtX0.y += 0;
        JtRtX0.z += e_cy * dz;
        //JtRtX1.x += 0;
        JtRtX1.y += e_cy * dy;
        JtRtX1.z += e_cy * dx;

        // Compute S = K JtRtX

        // K = [ gamma+mu2 gamma gamma 0 0 0 ]
        //     [ gamma gamma+mu2 gamma 0 0 0 ]
        //     [ gamma gamma gamma+mu2 0 0 0 ]
        //     [ 0 0 0             mu2/2 0 0 ]
        //     [ 0 0 0             0 mu2/2 0 ]
        //     [ 0 0 0             0 0 mu2/2 ]
        // S0 = JtRtX0*mu2 + dot(JtRtX0,(gamma gamma gamma))
        // S1 = JtRtX1*mu2/2

        real e_mu2_bx2 = e->mu2_bx2[index1];
        CudaVec3<real> S0  = JtRtX0*e_mu2_bx2;
        S0 += (JtRtX0.x+JtRtX0.y+JtRtX0.z)*e->gamma_bx2[index1];
        CudaVec3<real> S1  = JtRtX1*(e_mu2_bx2*0.5f);

        // Jd = ( 0   0   0   0   0  cy )
        //      ( 0   0   0   0  cy   0 )
        //      ( 0   0   cy  0   0   0 )
        fD = (Rt.mulT(CudaVec3<real>::make(
                e_cy * S1.z,
                e_cy * S1.y,
                e_cy * S0.z)));
        // Jc = ( 0   0   0  dz   0 -dy )
        //      ( 0   dz  0   0 -dy   0 )
        //      ( 0   0  -dy  0  dz   0 )
        fC = (Rt.mulT(CudaVec3<real>::make(
                e_dz * S1.x - e_dy * S1.z,
                e_dz * S0.y - e_dy * S1.y,
                e_dz * S1.y - e_dy * S0.z)));
        // Jb = (Jbx  0   0  Jby  0  Jbz)
        //      ( 0  Jby  0  Jbx Jbz  0 )
        //      ( 0   0  Jbz  0  Jby Jbx)
        fB = (Rt.mulT(CudaVec3<real>::make(
                e_Jbx_bx * S0.x                                     + e_Jby_bx * S1.x                   + e_Jbz_bx * S1.z,
                e_Jby_bx * S0.y                   + e_Jbx_bx * S1.x + e_Jbz_bx * S1.y,
                e_Jbz_bx * S0.z                   + e_Jby_bx * S1.y + e_Jbx_bx * S1.z)));
        //fA.x = -(fB.x+fC.x+fD.x);
        //fA.y = -(fB.y+fC.y+fD.y);
        //fA.z = -(fB.z+fC.z+fD.z);
    }

    //state[index] = s;
    Rt.writeAoS(rotations);
    //((rmatrix3*)rotations)[index] = Rt;
    //((GPUElementForce<real>*)eforce)[index] = f;

    //! Dynamically allocated shared memory to reorder global memory access
    __shared__  real temp[BSIZE*13];
    int index13 = umul24(index1,13);
    temp[index13+0 ] = -(fB.x+fC.x+fD.x);
    temp[index13+1 ] = -(fB.y+fC.y+fD.y);
    temp[index13+2 ] = -(fB.z+fC.z+fD.z);
    temp[index13+3 ] = fB.x;
    temp[index13+4 ] = fB.y;
    temp[index13+5 ] = fB.z;
    temp[index13+6 ] = fC.x;
    temp[index13+7 ] = fC.y;
    temp[index13+8 ] = fC.z;
    temp[index13+9 ] = fD.x;
    temp[index13+10] = fD.y;
    temp[index13+11] = fD.z;
    __syncthreads();
    real* out = ((real*)eforce)+(umul24(blockIdx.x,BSIZE*16))+index1;
    real v = 0;
    bool read = true; //(index1&4)<3;
    index1 += (index1>>4) - (index1>>2); // remove one for each 4-values before this thread, but add an extra one each 16 threads (so each 12 input cells, to align to 13)

    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;

}

template<typename real>
__global__ void MultiTetrahedronFEMForceFieldCuda3t_addForce_kernel()
{
    int nbVertex = multiTetraFEMForceOps[blockIdx.y].nbVertex;
    unsigned int nbElemPerVertex = multiTetraFEMForceOps[blockIdx.y].nbElemPerVertex;
    const CudaVec4<real>* eforce = (const CudaVec4<real>*) multiTetraFEMForceOps[blockIdx.y].eforce;
    const int* velems = (const int*) multiTetraFEMForceOps[blockIdx.y].velems;
    real* f = (real*) multiTetraFEMForceOps[blockIdx.y].f;

    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    if (index0 >= nbVertex) return;
    int index1 = threadIdx.x;
    int index3 = umul24(index1,3); //3*index1;

    //! Dynamically allocated shared memory to reorder global memory access
    __shared__  real temp[BSIZE*3];

    // First copy x inside temp
    int iext = umul24(blockIdx.x,BSIZE*3)+index1; //index0*3+index1;

    CudaVec3<real> force = CudaVec3<real>::make(0.0f,0.0f,0.0f);

    velems+=umul24(index0,nbElemPerVertex)+index1;

    if (index0+index1 < nbVertex)
        for (int s = 0; s < nbElemPerVertex; s++)
        {
            int i = *velems -1;
            velems+=BSIZE;
            if (i != -1)
            {
                force -= CudaTetrahedronFEMForceFieldTempTextures<real,CudaVec4<real> >::getElementForce(i,eforce);
            }
        }

    temp[index3  ] = force.x;
    temp[index3+1] = force.y;
    temp[index3+2] = force.z;

    __syncthreads();

    f[iext        ] += temp[index1        ];
    f[iext+  BSIZE] += temp[index1+  BSIZE];
    f[iext+2*BSIZE] += temp[index1+2*BSIZE];
}


template<typename real>
__global__ void MultiTetrahedronFEMForceFieldCuda3t_addDForce_kernel()
{
    int nbVertex = multiTetraFEMDForceOps[blockIdx.y].nbVertex;
    unsigned int nbElemPerVertex = multiTetraFEMDForceOps[blockIdx.y].nbElemPerVertex;
    const CudaVec4<real>* eforce = (const CudaVec4<real>*) multiTetraFEMDForceOps[blockIdx.y].eforce;
    const int* velems = (const int*) multiTetraFEMDForceOps[blockIdx.y].velems;
    real* f = (real*) multiTetraFEMDForceOps[blockIdx.y].df;

    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    if (index0 >= nbVertex) return;
    int index1 = threadIdx.x;
    int index3 = umul24(index1,3); //3*index1;

    //! Dynamically allocated shared memory to reorder global memory access
    __shared__  real temp[BSIZE*3];

    // First copy x inside temp
    int iext = umul24(blockIdx.x,BSIZE*3)+index1; //index0*3+index1;

    CudaVec3<real> force = CudaVec3<real>::make(0.0f,0.0f,0.0f);

    velems+=umul24(index0,nbElemPerVertex)+index1;

    if (index0+index1 < nbVertex)
        for (int s = 0; s < nbElemPerVertex; s++)
        {
            int i = *velems -1;
            velems+=BSIZE;
            if (i != -1)
            {
                force -= CudaTetrahedronFEMForceFieldTempTextures<real,CudaVec4<real> >::getElementForce(i,eforce);
            }
        }

    temp[index3  ] = force.x;
    temp[index3+1] = force.y;
    temp[index3+2] = force.z;

    __syncthreads();

    f[iext        ] += temp[index1        ];
    f[iext+  BSIZE] += temp[index1+  BSIZE];
    f[iext+2*BSIZE] += temp[index1+2*BSIZE];
}

template<typename real, class TIn>
__global__ void MultiTetrahedronFEMForceFieldCuda3t_calcDForce_kernel(real factor)
{
    int nbElem = multiTetraFEMDForceOps[blockIdx.y].nbElem;
    const GPUElement<real>* elems = (const GPUElement<real>*) multiTetraFEMDForceOps[blockIdx.y].elems;
    const real* rotations = (const real*) multiTetraFEMDForceOps[blockIdx.y].rotations;
    real* eforce = (real*) multiTetraFEMDForceOps[blockIdx.y].eforce;
    const TIn* x = (const TIn*) multiTetraFEMDForceOps[blockIdx.y].dx;

    int index0 = umul24(blockIdx.x,BSIZE); //blockDim.x;
    if (index0 >= nbElem) return;

    int index1 = threadIdx.x;
    int index = index0+index1;

    //GPUElement<real> e = elems[index];
    const GPUElement<real>* e = elems + blockIdx.x;
    //GPUElementState<real> s = state[index];
    //GPUElementForce<real> f;
    CudaVec3<real> fB,fC,fD;
    matrix3<real> Rt;
    rotations += umul24(index0,9)+index1;
    Rt.readAoS(rotations);
    //Rt = ((const rmatrix3*)rotations)[index];

    if (index < nbElem)
    {
        // Compute JtRtX = JbtRtB + JctRtC + JdtRtD

        CudaVec3<real> A = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->ia[index1], x); //((const CudaVec3<real>*)x)[e.ia];
        CudaVec3<real> JtRtX0,JtRtX1;


        CudaVec3<real> B = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->ib[index1], x); //((const CudaVec3<real>*)x)[e.ib];
        B = Rt * (B-A);

        // Jtb = (Jbx  0   0 )
        //       ( 0  Jby  0 )
        //       ( 0   0  Jbz)
        //       (Jby Jbx  0 )
        //       ( 0  Jbz Jby)
        //       (Jbz  0  Jbx)
        real e_Jbx_bx = e->Jbx_bx[index1];
        real e_Jby_bx = e->Jby_bx[index1];
        real e_Jbz_bx = e->Jbz_bx[index1];
        JtRtX0.x = e_Jbx_bx * B.x;
        JtRtX0.y =                  e_Jby_bx * B.y;
        JtRtX0.z =                                   e_Jbz_bx * B.z;
        JtRtX1.x = e_Jby_bx * B.x + e_Jbx_bx * B.y;
        JtRtX1.y =                  e_Jbz_bx * B.y + e_Jby_bx * B.z;
        JtRtX1.z = e_Jbz_bx * B.x                  + e_Jbx_bx * B.z;

        CudaVec3<real> C = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->ic[index1], x); //((const CudaVec3<real>*)x)[e.ic];
        C = Rt * (C-A);

        // Jtc = ( 0   0   0 )
        //       ( 0   dz  0 )
        //       ( 0   0  -dy)
        //       ( dz  0   0 )
        //       ( 0  -dy  dz)
        //       (-dy  0   0 )
        real e_dy = e->dy[index1];
        real e_dz = e->dz[index1];
        //JtRtX0.x += 0;
        JtRtX0.y +=              e_dz * C.y;
        JtRtX0.z +=                         - e_dy * C.z;
        JtRtX1.x += e_dz * C.x;
        JtRtX1.y +=            - e_dy * C.y + e_dz * C.z;
        JtRtX1.z -= e_dy * C.x;

        // Jtd = ( 0   0   0 )
        //       ( 0   0   0 )
        //       ( 0   0   cy)
        //       ( 0   0   0 )
        //       ( 0   cy  0 )
        //       ( cy  0   0 )
        CudaVec3<real> D = CudaTetrahedronFEMForceFieldInputTextures<real,TIn>::getDX(e->id[index1], x); //((const CudaVec3<real>*)x)[e.id];
        D = Rt * (D-A);

        real e_cy = e->cy[index1];
        //JtRtX0.x += 0;
        //JtRtX0.y += 0;
        JtRtX0.z +=                           e_cy * D.z;
        //JtRtX1.x += 0;
        JtRtX1.y +=              e_cy * D.y;
        JtRtX1.z += e_cy * D.x;

        // Compute S = K JtRtX

        // K = [ gamma+mu2 gamma gamma 0 0 0 ]
        //     [ gamma gamma+mu2 gamma 0 0 0 ]
        //     [ gamma gamma gamma+mu2 0 0 0 ]
        //     [ 0 0 0             mu2/2 0 0 ]
        //     [ 0 0 0             0 mu2/2 0 ]
        //     [ 0 0 0             0 0 mu2/2 ]
        // S0 = JtRtX0*mu2 + dot(JtRtX0,(gamma gamma gamma))
        // S1 = JtRtX1*mu2/2

        real e_mu2_bx2 = e->mu2_bx2[index1];
        CudaVec3<real> S0  = JtRtX0*e_mu2_bx2;
        S0 += (JtRtX0.x+JtRtX0.y+JtRtX0.z)*e->gamma_bx2[index1];
        CudaVec3<real> S1  = JtRtX1*(e_mu2_bx2*0.5f);

        S0 *= factor;
        S1 *= factor;

        // Jd = ( 0   0   0   0   0  cy )
        //      ( 0   0   0   0  cy   0 )
        //      ( 0   0   cy  0   0   0 )
        fD = (Rt.mulT(CudaVec3<real>::make(
                e_cy * S1.z,
                e_cy * S1.y,
                e_cy * S0.z)));
        // Jc = ( 0   0   0  dz   0 -dy )
        //      ( 0   dz  0   0 -dy   0 )
        //      ( 0   0  -dy  0  dz   0 )
        fC = (Rt.mulT(CudaVec3<real>::make(
                e_dz * S1.x - e_dy * S1.z,
                e_dz * S0.y - e_dy * S1.y,
                e_dz * S1.y - e_dy * S0.z)));
        // Jb = (Jbx  0   0  Jby  0  Jbz)
        //      ( 0  Jby  0  Jbx Jbz  0 )
        //      ( 0   0  Jbz  0  Jby Jbx)
        fB = (Rt.mulT(CudaVec3<real>::make(
                e_Jbx_bx * S0.x                                     + e_Jby_bx * S1.x                   + e_Jbz_bx * S1.z,
                e_Jby_bx * S0.y                   + e_Jbx_bx * S1.x + e_Jbz_bx * S1.y,
                e_Jbz_bx * S0.z                   + e_Jby_bx * S1.y + e_Jbx_bx * S1.z)));
        //fA.x = -(fB.x+fC.x+fD.x);
        //fA.y = -(fB.y+fC.y+fD.y);
        //fA.z = -(fB.z+fC.z+fD.z);
    }

    //state[index] = s;
    //((GPUElementForce<real>*)eforce)[index] = f;

    //! Dynamically allocated shared memory to reorder global memory access
    __shared__  real temp[BSIZE*13];
    int index13 = umul24(index1,13);
    temp[index13+0 ] = -(fB.x+fC.x+fD.x);
    temp[index13+1 ] = -(fB.y+fC.y+fD.y);
    temp[index13+2 ] = -(fB.z+fC.z+fD.z);
    temp[index13+3 ] = fB.x;
    temp[index13+4 ] = fB.y;
    temp[index13+5 ] = fB.z;
    temp[index13+6 ] = fC.x;
    temp[index13+7 ] = fC.y;
    temp[index13+8 ] = fC.z;
    temp[index13+9 ] = fD.x;
    temp[index13+10] = fD.y;
    temp[index13+11] = fD.z;
    __syncthreads();
    real* out = ((real*)eforce)+(umul24(blockIdx.x,BSIZE*16))+index1;
    real v = 0;
    bool read = true; //(index1&4)<3;
    index1 += (index1>>4) - (index1>>2); // remove one for each 4-values before this thread, but add an extra one each 16 threads (so each 12 input cells, to align to 13)

    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
    if (read) v = temp[index1]; index1 += (BSIZE+(BSIZE>>4)-(BSIZE>>2));
    *out = v; out += BSIZE;
}

//////////////////////
// CPU-side methods //
//////////////////////

void TetrahedronFEMForceFieldCuda3f_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
{
    CudaTetrahedronFEMForceFieldInputTextures<float,CudaVec3<float> >::setX(x);
    CudaTetrahedronFEMForceFieldTempTextures<float,CudaVec4<float> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcForce_kernel<float, CudaVec3<float> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<float>*)elems, (float*)state, (float*)eforce, (const CudaVec3<float>*)x);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addForce_kernel<float><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<float>*)eforce, (const int*)velems, (float*)f);
}

void TetrahedronFEMForceFieldCuda3f_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
{
    CudaTetrahedronFEMForceFieldInputTextures<float,CudaVec3<float> >::setDX(dx);
    CudaTetrahedronFEMForceFieldTempTextures<float,CudaVec4<float> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcDForce_kernel<float, CudaVec3<float> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<float>*)elems, (const float*)state, (float*)eforce, (const CudaVec3<float>*)dx, (float) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addForce_kernel<float><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<float>*)eforce, (const int*)velems, (float*)df);
}

void TetrahedronFEMForceFieldCuda3f_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
{
    CudaTetrahedronFEMForceFieldTempTextures<float,CudaVec4<float> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addKToMatrix_kernel<float, CudaVec3<float> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<float>*)elems, (const float*)state, (float*)eforce, (float) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addForce_kernel<float><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<float>*)eforce, (const int*)velems, (float*)df);
}

void TetrahedronFEMForceFieldCuda3f1_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
{
    CudaTetrahedronFEMForceFieldInputTextures<float,CudaVec4<float> >::setX(x);
    CudaTetrahedronFEMForceFieldTempTextures<float,CudaVec4<float> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcForce_kernel<float, CudaVec4<float> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<float>*)elems, (float*)state, (float*)eforce, (const CudaVec4<float>*)x);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t1_addForce_kernel<float><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<float>*)eforce, (const int*)velems, (CudaVec4<float>*)f);
}

void TetrahedronFEMForceFieldCuda3f1_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
{
    CudaTetrahedronFEMForceFieldInputTextures<float,CudaVec4<float> >::setDX(dx);
    CudaTetrahedronFEMForceFieldTempTextures<float,CudaVec4<float> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcDForce_kernel<float, CudaVec4<float> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<float>*)elems, (const float*)state, (float*)eforce, (const CudaVec4<float>*)dx, (float) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t1_addForce_kernel<float><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<float>*)eforce, (const int*)velems, (CudaVec4<float>*)df);
}

void TetrahedronFEMForceFieldCuda3f1_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
{
    CudaTetrahedronFEMForceFieldTempTextures<float,CudaVec4<float> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addKToMatrix_kernel<float, CudaVec4<float> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<float>*)elems, (const float*)state, (float*)eforce, (float) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t1_addForce_kernel<float><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<float>*)eforce, (const int*)velems, (CudaVec4<float>*)df);
}

void TetrahedronFEMForceFieldCuda3f_getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations)
{
    dim3 threads(BSIZE,1);
    dim3 grid((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_getRotations_kernel<float><<< grid, threads >>>(nbVertex, (const float*)initState, (const float*)state, (const int*)rotationIdx, (float*)rotations);
}


void MultiTetrahedronFEMForceFieldCuda3f_addForce(int n, TetraFEMForceOp* ops)
{
    int totalblocs1 = 0;
    int totalblocs2 = 0;
    for (unsigned int i0 = 0; i0 < n; i0 += MULTI_NMAX)
    {
        int n2 = (n-i0 > MULTI_NMAX) ? MULTI_NMAX : n-i0;
        int maxnblocs1 = 0;
        int maxnblocs2 = 0;
        TetraFEMForceOp* ops2 = ops + i0;
        for (unsigned int i = 0; i < n2; ++i)
        {
            int nblocs1 = (ops2[i].nbElem+BSIZE-1)/BSIZE;
            if (nblocs1 > maxnblocs1) maxnblocs1 = nblocs1;
            int nblocs2 = (ops2[i].nbVertex+BSIZE-1)/BSIZE;
            if (nblocs2 > maxnblocs2) maxnblocs2 = nblocs2;
        }
        cudaMemcpyToSymbol(multiTetraFEMForceOps, ops2, n2 * sizeof(TetraFEMForceOp), 0, cudaMemcpyHostToDevice);
        if (sofa::gpu::cuda::mycudaVerboseLevel >= sofa::gpu::cuda::LOG_TRACE)
            sofa::gpu::cuda::myprintf("multiforce(TetraFEM): %dx%d + %dx%d grid\n", maxnblocs1, n2, maxnblocs2, n2);
        dim3 threads1(BSIZE,1);
        dim3 grid1(maxnblocs1,n2);
        MultiTetrahedronFEMForceFieldCuda3t_calcForce_kernel<float, CudaVec3<float> ><<< grid1, threads1 >>>();
        dim3 threads2(BSIZE,1);
        dim3 grid2(maxnblocs2,n2);
        MultiTetrahedronFEMForceFieldCuda3t_addForce_kernel<float><<< grid2, threads2 >>>();

        totalblocs1 += maxnblocs1 * n2;
        totalblocs2 += maxnblocs2 * n2;
    }
}

void MultiTetrahedronFEMForceFieldCuda3f_addDForce(int n, TetraFEMDForceOp* ops, double factor)
{
    int totalblocs1 = 0;
    int totalblocs2 = 0;
    for (unsigned int i0 = 0; i0 < n; i0 += MULTI_NMAX)
    {
        int n2 = (n-i0 > MULTI_NMAX) ? MULTI_NMAX : n-i0;
        int maxnblocs1 = 0;
        int maxnblocs2 = 0;
        TetraFEMDForceOp* ops2 = ops + i0;
        for (unsigned int i = 0; i < n2; ++i)
        {
            int nblocs1 = (ops2[i].nbElem+BSIZE-1)/BSIZE;
            if (nblocs1 > maxnblocs1) maxnblocs1 = nblocs1;
            int nblocs2 = (ops2[i].nbVertex+BSIZE-1)/BSIZE;
            if (nblocs2 > maxnblocs2) maxnblocs2 = nblocs2;
        }
        cudaMemcpyToSymbol(multiTetraFEMDForceOps, ops2, n2 * sizeof(TetraFEMDForceOp), 0, cudaMemcpyHostToDevice);
        if (sofa::gpu::cuda::mycudaVerboseLevel >= sofa::gpu::cuda::LOG_TRACE)
            sofa::gpu::cuda::myprintf("multidforce(TetraFEM): %dx%d + %dx%d grid\n", maxnblocs1, n2, maxnblocs2, n2);
        dim3 threads1(BSIZE,1);
        dim3 grid1(maxnblocs1,n2);
        MultiTetrahedronFEMForceFieldCuda3t_calcDForce_kernel<float, CudaVec3<float> ><<< grid1, threads1 >>>(factor);
        dim3 threads2(BSIZE,1);
        dim3 grid2(maxnblocs2,n2);
        MultiTetrahedronFEMForceFieldCuda3t_addDForce_kernel<float><<< grid2, threads2 >>>();

        totalblocs1 += maxnblocs1 * n2;
        totalblocs2 += maxnblocs2 * n2;
    }
}

#ifdef SOFA_GPU_CUDA_DOUBLE

void TetrahedronFEMForceFieldCuda3d_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
{
    CudaTetrahedronFEMForceFieldInputTextures<double,CudaVec3<double> >::setX(x);
    CudaTetrahedronFEMForceFieldTempTextures<double,CudaVec4<double> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcForce_kernel<double, CudaVec3<double> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<double>*)elems, (double*)state, (double*)eforce, (const CudaVec3<double>*)x);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addForce_kernel<double><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<double>*)eforce, (const int*)velems, (double*)f);
}

void TetrahedronFEMForceFieldCuda3d_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
{
    CudaTetrahedronFEMForceFieldInputTextures<double,CudaVec3<double> >::setDX(dx);
    CudaTetrahedronFEMForceFieldTempTextures<double,CudaVec4<double> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcDForce_kernel<double, CudaVec3<double> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<double>*)elems, (const double*)state, (double*)eforce, (const CudaVec3<double>*)dx, (double) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addForce_kernel<double><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<double>*)eforce, (const int*)velems, (double*)df);
}

void TetrahedronFEMForceFieldCuda3d_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
{
    CudaTetrahedronFEMForceFieldTempTextures<double,CudaVec4<double> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addKToMatrix_kernel<double, CudaVec3<double> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<double>*)elems, (const double*)state, (double*)eforce, (double) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addForce_kernel<double><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<double>*)eforce, (const int*)velems, (double*)df);
}

void TetrahedronFEMForceFieldCuda3d1_addForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, void* state, void* eforce, const void* velems, void* f, const void* x, const void* v)
{
    CudaTetrahedronFEMForceFieldInputTextures<double,CudaVec4<double> >::setX(x);
    CudaTetrahedronFEMForceFieldTempTextures<double,CudaVec4<double> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcForce_kernel<double, CudaVec4<double> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<double>*)elems, (double*)state, (double*)eforce, (const CudaVec4<double>*)x);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t1_addForce_kernel<double><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<double>*)eforce, (const int*)velems, (CudaVec4<double>*)f);
}

void TetrahedronFEMForceFieldCuda3d1_addDForce(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, const void* dx, double factor)
{
    CudaTetrahedronFEMForceFieldInputTextures<double,CudaVec4<double> >::setDX(dx);
    CudaTetrahedronFEMForceFieldTempTextures<double,CudaVec4<double> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_calcDForce_kernel<double, CudaVec4<double> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<double>*)elems, (const double*)state, (double*)eforce, (const CudaVec4<double>*)dx, (double) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t1_addForce_kernel<double><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<double>*)eforce, (const int*)velems, (CudaVec4<double>*)df);
}

void TetrahedronFEMForceFieldCuda3d1_addKToMatrix(unsigned int nbElem, unsigned int nbVertex, unsigned int nbElemPerVertex, const void* elems, const void* state, void* eforce, const void* velems, void* df, double factor)
{
    CudaTetrahedronFEMForceFieldTempTextures<double,CudaVec4<double> >::setElementForce(eforce);
    dim3 threads1(BSIZE,1);
    dim3 grid1((nbElem+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_addKToMatrix_kernel<double, CudaVec4<double> ><<< grid1, threads1 >>>(nbElem, (const GPUElement<double>*)elems, (const double*)state, (double*)eforce, (double) factor);
    dim3 threads2(BSIZE,1);
    dim3 grid2((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t1_addForce_kernel<double><<< grid2, threads2 >>>(nbVertex, nbElemPerVertex, (const CudaVec4<double>*)eforce, (const int*)velems, (CudaVec4<double>*)df);
}

void TetrahedronFEMForceFieldCuda3d_getRotations(unsigned int nbElem, unsigned int nbVertex, const void* initState, const void* state, const void* rotationIdx, void* rotations)
{
    dim3 threads(BSIZE,1);
    dim3 grid((nbVertex+BSIZE-1)/BSIZE,1);
    TetrahedronFEMForceFieldCuda3t_getRotations_kernel<double><<< grid, threads >>>(nbVertex, (const double*)initState, (const double*)state, (const int*)rotationIdx, (double*)rotations);
}

#endif // SOFA_GPU_CUDA_DOUBLE

#if defined(__cplusplus)
} // namespace cuda
} // namespace gpu
} // namespace sofa
#endif
