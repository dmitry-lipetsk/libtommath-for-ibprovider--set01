////////////////////////////////////////////////////////////////////////////////
#include <_pch_.h>
#pragma hdrstop

#include "source/external/libtommath/set01/ibp_external__libtommatch_set01__tommath_private.h"
#include <structure/t_memory.h>

namespace ibp{namespace external{namespace libtommath{namespace set01{
////////////////////////////////////////////////////////////////////////////////

#ifndef MP_CFG__USE_DEBUG_MEM
# error MP_CFG__USE_DEBUG_MEM not defined!
#endif

////////////////////////////////////////////////////////////////////////////////

#if(!MP_CFG__USE_DEBUG_MEM)

void* mp_mem__malloc(size_t const n)
{
 return malloc(n);
}//mp_mem__malloc

//------------------------------------------------------------------------
void mp_mem__free(void* const p)
{
 return free(p);
}//mp_mem__free

#else

//------------------------------------------------------------------------
class mp_debug_mem_data
{
 public:
  static const size_t c_guard_block_size    =1024;
  static const size_t c_align_size          =32;

  static const unsigned char c_guard_byte1  =0xED;
  static const unsigned char c_guard_byte2  =0xDE;

  static const unsigned char c_init_byte    =0xCB;
  static const unsigned char c_align_byte   =0xBC;

  static const GUID c_block1_sign;
  static const GUID c_block2_sign;

 public:
  struct tag_guard_block
  {
   GUID   sign;
   size_t total_size;
   size_t user_size;
  };//struct tag_guard_block
};//class mp_debug_mem_data

//------------------------------------------------------------------------
const GUID mp_debug_mem_data::c_block1_sign=
 {0x77374134,0x56A6,0x47FB,{0xA2,0x62,0x3D,0xF8,0xBF,0x92,0x19,0xC4}};

const GUID mp_debug_mem_data::c_block2_sign=
 {0x9E6ABE1C,0xE954,0x4008,{0xB9,0x53,0xA6,0x3F,0x43,0xE7,0xC3,0x75}};

//------------------------------------------------------------------------
void* mp_mem__malloc(size_t const n)
{
 assert_s(sizeof(mp_debug_mem_data::tag_guard_block)<mp_debug_mem_data::c_guard_block_size);

 if(n==0)
  return NULL;

 size_t n1=0;

 //смещение первого сторожевого блока
 const size_t offset__guard1=n1;

 if(!structure::append_memory_size(n1,mp_debug_mem_data::c_guard_block_size))
  return NULL;

 //смещение блока с пользовательскими данными
 const size_t offset__user_data=n1;

 if(!structure::append_memory_size(n1,n))
  return NULL;

 //смещение выравнивающего блока
 const size_t offset__align=n1;

 if(!structure::align_memory_size(n1,mp_debug_mem_data::c_align_size))
  return NULL;

 //смещение второго сторожевого блока
 const size_t offset__guard2=n1;

 if(!structure::append_memory_size(n1,mp_debug_mem_data::c_guard_block_size))
  return NULL;

 unsigned char* const pBlock=reinterpret_cast<unsigned char*>(malloc(n1));

 if(pBlock == NULL)
  return NULL;

 //----------------------------------------- инициализация
 unsigned char* ptr=pBlock+offset__guard1;

 {
  mp_debug_mem_data::tag_guard_block* const pGuardBlock1
   =reinterpret_cast<mp_debug_mem_data::tag_guard_block*>(ptr);

  pGuardBlock1->sign      =mp_debug_mem_data::c_block1_sign;
  pGuardBlock1->total_size=n1;
  pGuardBlock1->user_size =n;
 }//if

 ptr+=sizeof(mp_debug_mem_data::tag_guard_block);

 std::fill(ptr,
           pBlock+offset__user_data,
           mp_debug_mem_data::c_guard_byte1);

 //-----------------------------------------
 ptr=pBlock+offset__user_data;

 std::fill(ptr,
           pBlock+offset__align,
           mp_debug_mem_data::c_init_byte);

 //-----------------------------------------
 ptr=pBlock+offset__align;

 std::fill(ptr,
           pBlock+offset__guard2,
           mp_debug_mem_data::c_align_byte);

 //-----------------------------------------
 ptr=pBlock+offset__guard2;

 {
  mp_debug_mem_data::tag_guard_block* const pGuardBlock2
   =reinterpret_cast<mp_debug_mem_data::tag_guard_block*>(ptr);

  pGuardBlock2->sign      =mp_debug_mem_data::c_block2_sign;
  pGuardBlock2->total_size=n1;
  pGuardBlock2->user_size =n;
 }

 ptr+=sizeof(mp_debug_mem_data::tag_guard_block);

 std::fill(ptr,
           pBlock+n1,
           mp_debug_mem_data::c_guard_byte2);

 //-----------------------------------------
 return pBlock+offset__user_data;
}//mp_mem__malloc

//------------------------------------------------------------------------
void mp_mem__free(void* const p)
{
 if(p==NULL)
  return;

 //-----------------------------------------
 const unsigned char*
  const pUserBlock=reinterpret_cast<const unsigned char*>(p);

 assert(mp_debug_mem_data::c_guard_block_size<=(size_t)(p));

 const unsigned char*
  const pBlock=(pUserBlock-mp_debug_mem_data::c_guard_block_size);

 //-----------------------------------------
 const mp_debug_mem_data::tag_guard_block*
  const pGuardBlock1=reinterpret_cast<const mp_debug_mem_data::tag_guard_block*>(pBlock);

 assert(IsEqualGUID(pGuardBlock1->sign,mp_debug_mem_data::c_block1_sign));

 assert(pGuardBlock1->user_size<pGuardBlock1->total_size);
 assert((2*mp_debug_mem_data::c_guard_block_size)<=(pGuardBlock1->total_size-pGuardBlock1->user_size));

 //-----------------------------------------
 for(size_t i=sizeof(mp_debug_mem_data::tag_guard_block);i!=mp_debug_mem_data::c_guard_block_size;++i)
 {
  assert(pBlock[i]==mp_debug_mem_data::c_guard_byte1);
 }//for

 //-----------------------------------------
 size_t n1=mp_debug_mem_data::c_guard_block_size;

 _VERIFY(structure::append_memory_size(n1,pGuardBlock1->user_size));

 const size_t offset__align=n1;

 _VERIFY(structure::align_memory_size(n1,mp_debug_mem_data::c_align_size));

 for(size_t i=offset__align;i!=n1;++i)
 {
  assert(pBlock[i]==mp_debug_mem_data::c_align_byte);
 }

 //-----------------------------------------
 const size_t offset__guard2=n1;

 const mp_debug_mem_data::tag_guard_block*
  const pGuardBlock2=reinterpret_cast<const mp_debug_mem_data::tag_guard_block*>(pBlock+offset__guard2);

 assert(IsEqualGUID(pGuardBlock2->sign,mp_debug_mem_data::c_block2_sign));

 assert(pGuardBlock2->total_size==pGuardBlock1->total_size);
 assert(pGuardBlock2->user_size==pGuardBlock1->user_size);

 //----------
 _VERIFY(structure::append_memory_size(n1,mp_debug_mem_data::c_guard_block_size));

 for(size_t i=offset__guard2+sizeof(mp_debug_mem_data::tag_guard_block);i!=n1;++i)
 {
  assert(pBlock[i]==mp_debug_mem_data::c_guard_byte2);
 }

 //-----------------------------------------
 assert(pGuardBlock1->total_size==n1);

 return free(const_cast<unsigned char*>(pBlock));
}//mp_mem__free

//------------------------------------------------------------------------
#ifndef NDEBUG

void mp_mem__debug_check_block(const void* const p)
{
 if(p==NULL)
  return;

 //-----------------------------------------
 const unsigned char*
  const pUserBlock=reinterpret_cast<const unsigned char*>(p);

 assert(mp_debug_mem_data::c_guard_block_size<=(size_t)(p));

 const unsigned char*
  const pBlock=(pUserBlock-mp_debug_mem_data::c_guard_block_size);

 //-----------------------------------------
 const mp_debug_mem_data::tag_guard_block*
  const pGuardBlock1=reinterpret_cast<const mp_debug_mem_data::tag_guard_block*>(pBlock);

 assert(IsEqualGUID(pGuardBlock1->sign,mp_debug_mem_data::c_block1_sign));

 assert(pGuardBlock1->user_size<pGuardBlock1->total_size);
 assert((2*mp_debug_mem_data::c_guard_block_size)<=(pGuardBlock1->total_size-pGuardBlock1->user_size));

 //-----------------------------------------
 for(size_t i=sizeof(mp_debug_mem_data::tag_guard_block);i!=mp_debug_mem_data::c_guard_block_size;++i)
 {
  assert(pBlock[i]==mp_debug_mem_data::c_guard_byte1);
 }//for

 //-----------------------------------------
 size_t n1=mp_debug_mem_data::c_guard_block_size;

 _VERIFY(structure::append_memory_size(n1,pGuardBlock1->user_size));

 const size_t offset__align=n1;

 _VERIFY(structure::align_memory_size(n1,mp_debug_mem_data::c_align_size));

 for(size_t i=offset__align;i!=n1;++i)
 {
  assert(pBlock[i]==mp_debug_mem_data::c_align_byte);
 }

 //-----------------------------------------
 const size_t offset__guard2=n1;

 const mp_debug_mem_data::tag_guard_block*
  const pGuardBlock2=reinterpret_cast<const mp_debug_mem_data::tag_guard_block*>(pBlock+offset__guard2);

 assert(IsEqualGUID(pGuardBlock2->sign,mp_debug_mem_data::c_block2_sign));

 assert(pGuardBlock2->total_size==pGuardBlock1->total_size);
 assert(pGuardBlock2->user_size==pGuardBlock1->user_size);

 //----------
 _VERIFY(structure::append_memory_size(n1,mp_debug_mem_data::c_guard_block_size));

 for(size_t i=offset__guard2+sizeof(mp_debug_mem_data::tag_guard_block);i!=n1;++i)
 {
  assert(pBlock[i]==mp_debug_mem_data::c_guard_byte2);
 }

 //-----------------------------------------
 assert(pGuardBlock1->total_size==n1);
}//mp_mem__debug_check_block

#endif // !NDEBUG

//------------------------------------------------------------------------
#endif

////////////////////////////////////////////////////////////////////////////////
//class mp_int_x

mp_int_x::mp_int_x()
{
 mp_init_v3(this);
}//mp_int_x

//------------------------------------------------------------------------
mp_int_x::~mp_int_x()
{
 mp_clear_v3(this);
}//~mp_int_x

////////////////////////////////////////////////////////////////////////////////
}/*nms set01*/}/*nms libtommath*/}/*nms external*/}/*nms ibp*/
