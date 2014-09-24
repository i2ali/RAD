#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /T ps_4_0_level_9_0 /E LabColorComparer /Fh LabColorComparerShader.h
//    .\LabColorComparer.hlsl
//
//
// Buffer Definitions: 
//
// cbuffer LabColorComparerParameters
// {
//
//   float4 inputColor;                 // Offset:    0 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// InputSampler                      sampler      NA          NA    0        1
// InputTexture                      texture  float4          2d    0        1
// LabColorComparerParameters        cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float       
// SCENE_POSITION           0   xyzw        1     NONE  float       
// TEXCOORD                 0   xyzw        2     NONE  float   xy  
// TEXCOORD                 1   xyzw        3     NONE  float       
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_Target                0   xyzw        0   TARGET  float   xyzw
//
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c0         cb0             0         1  ( FLT, FLT, FLT, FLT)
//
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0               
//
//
// Level9 shader bytecode:
//
    ps_2_0
    def c1, 0, 0.0450000018, 1, 0.0149999997
    def c2, 0, 0, 0, 1
    dcl t1
    dcl_2d s0
    texld r0, t1, s0
    add r0.w, r0.z, -c0.z
    mul r0.w, r0.w, r0.w
    add r1.w, r0.y, -c0.y
    mad r0.w, r1.w, r1.w, r0.w
    mov r1.x, c1.x
    dp2add r1.x, c0.yzxw, c0.yzxw, r1.x
    rsq r1.x, r1.x
    rcp r1.x, r1.x
    dp2add r0.y, r0.yzxw, r0.yzxw, c1.x
    add r0.x, r0.x, -c0.x
    rsq r0.y, r0.y
    rcp r0.y, r0.y
    add r0.z, -r1.x, r0.y
    mad r0.w, r0.z, -r0.z, r0.w
    mul r0.z, r0.z, r0.z
    rsq r1.x, r0.w
    rcp r1.x, r1.x
    cmp r0.w, -r0.w, c1.x, r1.x
    mul r0.w, r0.w, r0.w
    mad r1.x, r0.y, c1.y, c1.z
    mad r0.y, r0.y, c1.w, c1.z
    mul r0.y, r0.y, r0.y
    rcp r0.y, r0.y
    mul r1.x, r1.x, r1.x
    rcp r1.x, r1.x
    mul r0.z, r0.z, r1.x
    mad r0.x, r0.x, r0.x, r0.z
    mad r0.x, r0.w, r0.y, r0.x
    rsq r0.x, r0.x
    rcp r0.x, r0.x
    mov r0.yz, c2.y
    mov r0.w, c2.w
    mov oC0, r0

// approximately 36 instruction slots used (1 texture, 35 arithmetic)
//
// Constant buffer to DX9 shader constant mappings:
//
// Target Reg Buffer  Start Reg # of Regs        Data Conversion
// ---------- ------- --------- --------- ----------------------
// c0         cb0             0         1  ( FLT, FLT, FLT, FLT)
//
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0               
//
//
// XNA shader bytecode:
//
    ps_2_0
    def c1, 0, 0.0450000018, 0.0149999997, 1
    def c2, 0, 0, 0, 1
    dcl t1
    dcl_2d s0
    texld r0, r2, s0
    add r1.xyz, r0.yzxw, -c0.yzxw
    dp2add r1.w, r0.yzzw, r0.yzzw, c1.x
    rsq r1.w, r1.w
    rcp r1.w, r1.w
    mul r0.xyz, r1, r1
    add r0.x, r0.y, r0.x
    mov r1.x, c1.x
    dp2add r0.y, c0.yzzw, c0.yzzw, r1.x
    rsq r0.y, r0.y
    rcp r0.y, r0.y
    add r0.y, -r0.y, r1.w
    mad r1.xy, r1.w, c1.yzzw, c1.w
    mul r1.xy, r1, r1
    mad r0.x, r0.y, -r0.y, r0.x
    rsq r0.w, r0.x
    rcp r0.w, r0.w
    cmp r0.x, -r0.x, c1.x, r0.w
    mul r0.xy, r0, r0
  + rcp r0.w, r1.x
    rcp r1.x, r1.y
    mad r0.y, r0.y, r0.w, r0.z
    mad r0.x, r0.x, r1.x, r0.y
    rsq r0.x, r0.x
    mov oC0.yzw, c2.xyyw
  + rcp oC0.x, r0.x

// approximately 26 instruction slots used (1 texture, 25 arithmetic)
ps_4_0
dcl_constantbuffer cb0[1], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v2.xy
dcl_output o0.xyzw
dcl_temps 2
dp2 r0.x, cb0[0].yzyy, cb0[0].yzyy
sample r1.xyzw, v2.xyxx, t0.xyzw, s0
dp2 r0.y, r1.yzyy, r1.yzyy
add r1.xyz, r1.yzxy, -cb0[0].yzxy
sqrt r0.xy, r0.xyxx
add r0.x, -r0.x, r0.y
mad r0.yz, r0.yyyy, l(0.000000, 0.045000, 0.015000, 0.000000), l(0.000000, 1.000000, 1.000000, 0.000000)
mul r1.xy, r1.xyxx, r1.xyxx
add r0.w, r1.y, r1.x
mad r0.w, -r0.x, r0.x, r0.w
mul r0.xyz, r0.xyzx, r0.xyzx
div r0.x, r0.x, r0.y
mad r0.x, r1.z, r1.z, r0.x
lt r0.y, l(0.000000), r0.w
sqrt r0.w, r0.w
and r0.y, r0.w, r0.y
mul r0.y, r0.y, r0.y
div r0.y, r0.y, r0.z
add r0.x, r0.y, r0.x
sqrt o0.x, r0.x
mov o0.yzw, l(0,0,0,1.000000)
ret 
// Approximately 22 instruction slots used
#endif

const BYTE g_LabColorComparer[] =
{
     68,  88,  66,  67,   0,  21, 
    218,  79, 169,  10, 180,  93, 
    100,  70, 220,   0, 134, 161, 
    243, 213,   1,   0,   0,   0, 
     84,  10,   0,   0,   7,   0, 
      0,   0,  60,   0,   0,   0, 
     84,   2,   0,   0, 236,   4, 
      0,   0, 212,   7,   0,   0, 
     80,   8,   0,   0, 140,   9, 
      0,   0,  32,  10,   0,   0, 
     88,  78,  65,  83,  16,   2, 
      0,   0,  16,   2,   0,   0, 
      0,   2, 255, 255, 220,   1, 
      0,   0,  52,   0,   0,   0, 
      1,   0,  40,   0,   0,   0, 
     52,   0,   0,   0,  52,   0, 
      1,   0,  36,   0,   0,   0, 
     52,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   2, 255, 255,  81,   0, 
      0,   5,   1,   0,  15, 160, 
      0,   0,   0,   0, 236,  81, 
     56,  61, 143, 194, 117,  60, 
      0,   0, 128,  63,  81,   0, 
      0,   5,   2,   0,  15, 160, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128,  63,  31,   0, 
      0,   2,   0,   0,   0, 128, 
      1,   0,  15, 176,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      0,   8,  15, 160,  66,   0, 
      0,   3,   0,   0,  15, 128, 
      2,   0, 228, 128,   0,   8, 
    228, 160,   2,   0,   0,   3, 
      1,   0,   7, 128,   0,   0, 
    201, 128,   0,   0, 201, 161, 
     90,   0,   0,   4,   1,   0, 
      8, 128,   0,   0, 233, 128, 
      0,   0, 233, 128,   1,   0, 
      0, 160,   7,   0,   0,   2, 
      1,   0,   8, 128,   1,   0, 
    255, 128,   6,   0,   0,   2, 
      1,   0,   8, 128,   1,   0, 
    255, 128,   5,   0,   0,   3, 
      0,   0,   7, 128,   1,   0, 
    228, 128,   1,   0, 228, 128, 
      2,   0,   0,   3,   0,   0, 
      1, 128,   0,   0,  85, 128, 
      0,   0,   0, 128,   1,   0, 
      0,   2,   1,   0,   1, 128, 
      1,   0,   0, 160,  90,   0, 
      0,   4,   0,   0,   2, 128, 
      0,   0, 233, 160,   0,   0, 
    233, 160,   1,   0,   0, 128, 
      7,   0,   0,   2,   0,   0, 
      2, 128,   0,   0,  85, 128, 
      6,   0,   0,   2,   0,   0, 
      2, 128,   0,   0,  85, 128, 
      2,   0,   0,   3,   0,   0, 
      2, 128,   0,   0,  85, 129, 
      1,   0, 255, 128,   4,   0, 
      0,   4,   1,   0,   3, 128, 
      1,   0, 255, 128,   1,   0, 
    233, 160,   1,   0, 255, 160, 
      5,   0,   0,   3,   1,   0, 
      3, 128,   1,   0, 228, 128, 
      1,   0, 228, 128,   4,   0, 
      0,   4,   0,   0,   1, 128, 
      0,   0,  85, 128,   0,   0, 
     85, 129,   0,   0,   0, 128, 
      7,   0,   0,   2,   0,   0, 
      8, 128,   0,   0,   0, 128, 
      6,   0,   0,   2,   0,   0, 
      8, 128,   0,   0, 255, 128, 
     88,   0,   0,   4,   0,   0, 
      1, 128,   0,   0,   0, 129, 
      1,   0,   0, 160,   0,   0, 
    255, 128,   5,   0,   0,   3, 
      0,   0,   3, 128,   0,   0, 
    228, 128,   0,   0, 228, 128, 
      6,   0,   0,  66,   0,   0, 
      8, 128,   1,   0,   0, 128, 
      6,   0,   0,   2,   1,   0, 
      1, 128,   1,   0,  85, 128, 
      4,   0,   0,   4,   0,   0, 
      2, 128,   0,   0,  85, 128, 
      0,   0, 255, 128,   0,   0, 
    170, 128,   4,   0,   0,   4, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   1,   0,   0, 128, 
      0,   0,  85, 128,   7,   0, 
      0,   2,   0,   0,   1, 128, 
      0,   0,   0, 128,   1,   0, 
      0,   2,   0,   8,  14, 128, 
      2,   0, 212, 160,   6,   0, 
      0,  66,   0,   8,   1, 128, 
      0,   0,   0, 128, 255, 255, 
      0,   0,  65, 111, 110,  57, 
    144,   2,   0,   0, 144,   2, 
      0,   0,   0,   2, 255, 255, 
     92,   2,   0,   0,  52,   0, 
      0,   0,   1,   0,  40,   0, 
      0,   0,  52,   0,   0,   0, 
     52,   0,   1,   0,  36,   0, 
      0,   0,  52,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   2, 255, 255, 
     81,   0,   0,   5,   1,   0, 
     15, 160,   0,   0,   0,   0, 
    236,  81,  56,  61,   0,   0, 
    128,  63, 143, 194, 117,  60, 
     81,   0,   0,   5,   2,   0, 
     15, 160,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   1,   0,  15, 176, 
     31,   0,   0,   2,   0,   0, 
      0, 144,   0,   8,  15, 160, 
     66,   0,   0,   3,   0,   0, 
     15, 128,   1,   0, 228, 176, 
      0,   8, 228, 160,   2,   0, 
      0,   3,   0,   0,   8, 128, 
      0,   0, 170, 128,   0,   0, 
    170, 161,   5,   0,   0,   3, 
      0,   0,   8, 128,   0,   0, 
    255, 128,   0,   0, 255, 128, 
      2,   0,   0,   3,   1,   0, 
      8, 128,   0,   0,  85, 128, 
      0,   0,  85, 161,   4,   0, 
      0,   4,   0,   0,   8, 128, 
      1,   0, 255, 128,   1,   0, 
    255, 128,   0,   0, 255, 128, 
      1,   0,   0,   2,   1,   0, 
      1, 128,   1,   0,   0, 160, 
     90,   0,   0,   4,   1,   0, 
      1, 128,   0,   0, 201, 160, 
      0,   0, 201, 160,   1,   0, 
      0, 128,   7,   0,   0,   2, 
      1,   0,   1, 128,   1,   0, 
      0, 128,   6,   0,   0,   2, 
      1,   0,   1, 128,   1,   0, 
      0, 128,  90,   0,   0,   4, 
      0,   0,   2, 128,   0,   0, 
    201, 128,   0,   0, 201, 128, 
      1,   0,   0, 160,   2,   0, 
      0,   3,   0,   0,   1, 128, 
      0,   0,   0, 128,   0,   0, 
      0, 161,   7,   0,   0,   2, 
      0,   0,   2, 128,   0,   0, 
     85, 128,   6,   0,   0,   2, 
      0,   0,   2, 128,   0,   0, 
     85, 128,   2,   0,   0,   3, 
      0,   0,   4, 128,   1,   0, 
      0, 129,   0,   0,  85, 128, 
      4,   0,   0,   4,   0,   0, 
      8, 128,   0,   0, 170, 128, 
      0,   0, 170, 129,   0,   0, 
    255, 128,   5,   0,   0,   3, 
      0,   0,   4, 128,   0,   0, 
    170, 128,   0,   0, 170, 128, 
      7,   0,   0,   2,   1,   0, 
      1, 128,   0,   0, 255, 128, 
      6,   0,   0,   2,   1,   0, 
      1, 128,   1,   0,   0, 128, 
     88,   0,   0,   4,   0,   0, 
      8, 128,   0,   0, 255, 129, 
      1,   0,   0, 160,   1,   0, 
      0, 128,   5,   0,   0,   3, 
      0,   0,   8, 128,   0,   0, 
    255, 128,   0,   0, 255, 128, 
      4,   0,   0,   4,   1,   0, 
      1, 128,   0,   0,  85, 128, 
      1,   0,  85, 160,   1,   0, 
    170, 160,   4,   0,   0,   4, 
      0,   0,   2, 128,   0,   0, 
     85, 128,   1,   0, 255, 160, 
      1,   0, 170, 160,   5,   0, 
      0,   3,   0,   0,   2, 128, 
      0,   0,  85, 128,   0,   0, 
     85, 128,   6,   0,   0,   2, 
      0,   0,   2, 128,   0,   0, 
     85, 128,   5,   0,   0,   3, 
      1,   0,   1, 128,   1,   0, 
      0, 128,   1,   0,   0, 128, 
      6,   0,   0,   2,   1,   0, 
      1, 128,   1,   0,   0, 128, 
      5,   0,   0,   3,   0,   0, 
      4, 128,   0,   0, 170, 128, 
      1,   0,   0, 128,   4,   0, 
      0,   4,   0,   0,   1, 128, 
      0,   0,   0, 128,   0,   0, 
      0, 128,   0,   0, 170, 128, 
      4,   0,   0,   4,   0,   0, 
      1, 128,   0,   0, 255, 128, 
      0,   0,  85, 128,   0,   0, 
      0, 128,   7,   0,   0,   2, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   6,   0,   0,   2, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   1,   0,   0,   2, 
      0,   0,   6, 128,   2,   0, 
     85, 160,   1,   0,   0,   2, 
      0,   0,   8, 128,   2,   0, 
    255, 160,   1,   0,   0,   2, 
      0,   8,  15, 128,   0,   0, 
    228, 128, 255, 255,   0,   0, 
     83,  72,  68,  82, 224,   2, 
      0,   0,  64,   0,   0,   0, 
    184,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   2,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   2,   0, 
      0,   0,  15,   0,   0,   9, 
     18,   0,  16,   0,   0,   0, 
      0,   0, 150, 133,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 150, 133,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      2,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  15,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0, 150,   5,  16,   0, 
      1,   0,   0,   0, 150,   5, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   9, 114,   0, 
     16,   0,   1,   0,   0,   0, 
    150,   4,  16,   0,   1,   0, 
      0,   0, 150, 132,  32, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     75,   0,   0,   5,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  15,  98,   0,  16,   0, 
      0,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0, 236,  81,  56,  61, 
    143, 194, 117,  60,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   7, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  10, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  14,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,   9,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     49,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  75,   0, 
      0,   5, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  14,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  75,   0, 
      0,   5,  18,  32,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   8, 226,  32, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,  22,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  82,  68, 
     69,  70,  52,   1,   0,   0, 
      1,   0,   0,   0, 180,   0, 
      0,   0,   3,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,   0,   1,   0,   0, 
      0,   1,   0,   0, 124,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    137,   0,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 150,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  73, 110, 
    112, 117, 116,  83,  97, 109, 
    112, 108, 101, 114,   0,  73, 
    110, 112, 117, 116,  84, 101, 
    120, 116, 117, 114, 101,   0, 
     76,  97,  98,  67, 111, 108, 
    111, 114,  67, 111, 109, 112, 
     97, 114, 101, 114,  80,  97, 
    114,  97, 109, 101, 116, 101, 
    114, 115,   0, 171, 171, 171, 
    150,   0,   0,   0,   1,   0, 
      0,   0, 204,   0,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    228,   0,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0, 240,   0, 
      0,   0,   0,   0,   0,   0, 
    105, 110, 112, 117, 116,  67, 
    111, 108, 111, 114,   0, 171, 
      1,   0,   3,   0,   1,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  57,  46,  50, 
     57,  46,  57,  53,  50,  46, 
     51,  49,  49,  49,   0, 171, 
    171, 171,  73,  83,  71,  78, 
    140,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   0,   0,   0, 
    131,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  15,   3,   0,   0, 
    131,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     83,  67,  69,  78,  69,  95, 
     80,  79,  83,  73,  84,  73, 
     79,  78,   0,  84,  69,  88, 
     67,  79,  79,  82,  68,   0, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  97, 114, 103, 101, 
    116,   0, 171, 171
};
