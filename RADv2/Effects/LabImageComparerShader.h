#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /T ps_4_0_level_9_0 /E LabImageComparer /Fh LabImageComparerShader.h
//    LabImageComparer.hlsl
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// InputSampler0                     sampler      NA          NA    0        1
// InputSampler1                     sampler      NA          NA    1        1
// InputTexture0                     texture  float4          2d    0        1
// InputTexture1                     texture  float4          2d    1        1
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
// TEXCOORD                 1   xyzw        3     NONE  float   xy  
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_Target                0   xyzw        0   TARGET  float   xyzw
//
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0               
// s1             s1              t1               
//
//
// Level9 shader bytecode:
//
    ps_2_0
    def c0, 0, 0.0450000018, 1, 0.0149999997
    def c1, 0, 0, 0, 1
    dcl t1
    dcl t2
    dcl_2d s0
    dcl_2d s1
    texld r0, t2, s1
    texld r1, t1, s0
    mul r0.w, r0.z, r0.z
    mad r0.w, r0.y, r0.y, r0.w
    rsq r0.w, r0.w
    rcp r0.w, r0.w
    mul r1.w, r1.z, r1.z
    mad r1.w, r1.y, r1.y, r1.w
    rsq r1.w, r1.w
    rcp r1.w, r1.w
    add r0, -r0, r1
    mul r0.z, r0.z, r0.z
    mad r0.y, r0.y, r0.y, r0.z
    mad r0.y, r0.w, -r0.w, r0.y
    mul r0.z, r0.w, r0.w
    rsq r0.w, r0.y
    rcp r0.w, r0.w
    cmp r0.y, -r0.y, c0.x, r0.w
    mul r0.y, r0.y, r0.y
    mad r0.w, r1.w, c0.y, c0.z
    mad r1.x, r1.w, c0.w, c0.z
    mul r1.x, r1.x, r1.x
    rcp r1.x, r1.x
    mul r0.w, r0.w, r0.w
    rcp r0.w, r0.w
    mul r0.z, r0.w, r0.z
    mad r0.x, r0.x, r0.x, r0.z
    mad r0.x, r0.y, r1.x, r0.x
    rsq r0.x, r0.x
    rcp r0.x, r0.x
    mov r0.yz, c1.y
    mov r0.w, c1.w
    mov oC0, r0

// approximately 33 instruction slots used (2 texture, 31 arithmetic)
//
// Sampler/Resource to DX9 shader sampler mappings:
//
// Target Sampler Source Sampler  Source Resource
// -------------- --------------- ----------------
// s0             s0              t0               
// s1             s1              t1               
//
//
// XNA shader bytecode:
//
    ps_2_0
    def c0, 0, 0.0450000018, 0.0149999997, 1
    def c1, 0, 0, 0, 1
    dcl t1
    dcl t2
    dcl_2d s0
    dcl_2d s1
    texld r0, r2, s0
    texld r1, r3, s1
    add r2.xyz, r0.yzxw, -r1.yzxw
    dp2add r2.w, r0.yzzw, r0.yzzw, c0.x
    rsq r2.w, r2.w
    rcp r2.w, r2.w
    dp2add r0.x, r1.yzzw, r1.yzzw, c0.x
    rsq r0.x, r0.x
    rcp r0.x, r0.x
    add r0.x, -r0.x, r2.w
    mad r0.yz, r2.w, c0, c0.w
    mul r1.xyz, r2, r2
    mul r0.yzw, r0.xyzx, r0.xyzx
    rcp r1.w, r0.y
    rcp r0.y, r0.z
    mad r0.z, r0.w, r1.w, r1.z
    add r0.w, r1.y, r1.x
    mad r0.x, r0.x, -r0.x, r0.w
    rsq r0.w, r0.x
    rcp r0.w, r0.w
    cmp r0.x, -r0.x, c0.x, r0.w
    mul r0.x, r0.x, r0.x
    mad r0.x, r0.x, r0.y, r0.z
    rsq r0.x, r0.x
    mov oC0.yzw, c1.xyyw
  + rcp oC0.x, r0.x

// approximately 27 instruction slots used (2 texture, 25 arithmetic)
ps_4_0
dcl_sampler s0, mode_default
dcl_sampler s1, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_texture2d (float,float,float,float) t1
dcl_input_ps linear v2.xy
dcl_input_ps linear v3.xy
dcl_output o0.xyzw
dcl_temps 2
sample r0.xyzw, v3.xyxx, t1.xyzw, s1
dp2 r0.w, r0.yzyy, r0.yzyy
sqrt r0.w, r0.w
sample r1.xyzw, v2.xyxx, t0.xyzw, s0
dp2 r1.w, r1.yzyy, r1.yzyy
add r0.xyz, -r0.yzxy, r1.yzxy
sqrt r1.x, r1.w
add r0.w, -r0.w, r1.x
mad r1.xy, r1.xxxx, l(0.045000, 0.015000, 0.000000, 0.000000), l(1.000000, 1.000000, 0.000000, 0.000000)
mul r1.xy, r1.xyxx, r1.xyxx
mul r0.xy, r0.xyxx, r0.xyxx
add r0.x, r0.y, r0.x
mad r0.x, -r0.w, r0.w, r0.x
mul r0.y, r0.w, r0.w
div r0.y, r0.y, r1.x
mad r0.y, r0.z, r0.z, r0.y
lt r0.z, l(0.000000), r0.x
sqrt r0.x, r0.x
and r0.x, r0.x, r0.z
mul r0.x, r0.x, r0.x
div r0.x, r0.x, r1.y
add r0.x, r0.x, r0.y
sqrt o0.x, r0.x
mov o0.yzw, l(0,0,0,1.000000)
ret 
// Approximately 25 instruction slots used
#endif

const BYTE g_LabImageComparer[] =
{
     68,  88,  66,  67, 155,  43, 
     13, 222,  41, 165, 163, 192, 
    217,  22, 156, 155, 165, 226, 
     34, 197,   1,   0,   0,   0, 
    160,  10,   0,   0,   7,   0, 
      0,   0,  60,   0,   0,   0, 
    104,   2,   0,   0,   4,   5, 
      0,   0,  76,   8,   0,   0, 
    200,   8,   0,   0, 216,   9, 
      0,   0, 108,  10,   0,   0, 
     88,  78,  65,  83,  36,   2, 
      0,   0,  36,   2,   0,   0, 
      0,   2, 255, 255, 248,   1, 
      0,   0,  44,   0,   0,   0, 
      0,   0,  44,   0,   0,   0, 
     44,   0,   0,   0,  44,   0, 
      2,   0,  36,   0,   0,   0, 
     44,   0,   0,   0,   0,   0, 
      1,   1,   1,   0,   0,   2, 
    255, 255,  81,   0,   0,   5, 
      0,   0,  15, 160,   0,   0, 
      0,   0, 236,  81,  56,  61, 
    143, 194, 117,  60,   0,   0, 
    128,  63,  81,   0,   0,   5, 
      1,   0,  15, 160,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,  31,   0,   0,   2, 
      0,   0,   0, 128,   1,   0, 
     15, 176,  31,   0,   0,   2, 
      0,   0,   0, 128,   2,   0, 
     15, 176,  31,   0,   0,   2, 
      0,   0,   0, 144,   0,   8, 
     15, 160,  31,   0,   0,   2, 
      0,   0,   0, 144,   1,   8, 
     15, 160,  66,   0,   0,   3, 
      0,   0,  15, 128,   2,   0, 
    228, 128,   0,   8, 228, 160, 
     66,   0,   0,   3,   1,   0, 
     15, 128,   3,   0, 228, 128, 
      1,   8, 228, 160,   2,   0, 
      0,   3,   2,   0,   7, 128, 
      0,   0, 201, 128,   1,   0, 
    201, 129,  90,   0,   0,   4, 
      2,   0,   8, 128,   0,   0, 
    233, 128,   0,   0, 233, 128, 
      0,   0,   0, 160,   7,   0, 
      0,   2,   2,   0,   8, 128, 
      2,   0, 255, 128,   6,   0, 
      0,   2,   2,   0,   8, 128, 
      2,   0, 255, 128,  90,   0, 
      0,   4,   0,   0,   1, 128, 
      1,   0, 233, 128,   1,   0, 
    233, 128,   0,   0,   0, 160, 
      7,   0,   0,   2,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      6,   0,   0,   2,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      2,   0,   0,   3,   0,   0, 
      1, 128,   0,   0,   0, 129, 
      2,   0, 255, 128,   4,   0, 
      0,   4,   0,   0,   6, 128, 
      2,   0, 255, 128,   0,   0, 
    228, 160,   0,   0, 255, 160, 
      5,   0,   0,   3,   1,   0, 
      7, 128,   2,   0, 228, 128, 
      2,   0, 228, 128,   5,   0, 
      0,   3,   0,   0,  14, 128, 
      0,   0,  36, 128,   0,   0, 
     36, 128,   6,   0,   0,   2, 
      1,   0,   8, 128,   0,   0, 
     85, 128,   6,   0,   0,   2, 
      0,   0,   2, 128,   0,   0, 
    170, 128,   4,   0,   0,   4, 
      0,   0,   4, 128,   0,   0, 
    255, 128,   1,   0, 255, 128, 
      1,   0, 170, 128,   2,   0, 
      0,   3,   0,   0,   8, 128, 
      1,   0,  85, 128,   1,   0, 
      0, 128,   4,   0,   0,   4, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   0,   0,   0, 129, 
      0,   0, 255, 128,   7,   0, 
      0,   2,   0,   0,   8, 128, 
      0,   0,   0, 128,   6,   0, 
      0,   2,   0,   0,   8, 128, 
      0,   0, 255, 128,  88,   0, 
      0,   4,   0,   0,   1, 128, 
      0,   0,   0, 129,   0,   0, 
      0, 160,   0,   0, 255, 128, 
      5,   0,   0,   3,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      0,   0,   0, 128,   4,   0, 
      0,   4,   0,   0,   1, 128, 
      0,   0,   0, 128,   0,   0, 
     85, 128,   0,   0, 170, 128, 
      7,   0,   0,   2,   0,   0, 
      1, 128,   0,   0,   0, 128, 
      1,   0,   0,   2,   0,   8, 
     14, 128,   1,   0, 212, 160, 
      6,   0,   0,  66,   0,   8, 
      1, 128,   0,   0,   0, 128, 
    255, 255,   0,   0,  65, 111, 
    110,  57, 148,   2,   0,   0, 
    148,   2,   0,   0,   0,   2, 
    255, 255, 104,   2,   0,   0, 
     44,   0,   0,   0,   0,   0, 
     44,   0,   0,   0,  44,   0, 
      0,   0,  44,   0,   2,   0, 
     36,   0,   0,   0,  44,   0, 
      0,   0,   0,   0,   1,   1, 
      1,   0,   0,   2, 255, 255, 
     81,   0,   0,   5,   0,   0, 
     15, 160,   0,   0,   0,   0, 
    236,  81,  56,  61,   0,   0, 
    128,  63, 143, 194, 117,  60, 
     81,   0,   0,   5,   1,   0, 
     15, 160,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   1,   0,  15, 176, 
     31,   0,   0,   2,   0,   0, 
      0, 128,   2,   0,  15, 176, 
     31,   0,   0,   2,   0,   0, 
      0, 144,   0,   8,  15, 160, 
     31,   0,   0,   2,   0,   0, 
      0, 144,   1,   8,  15, 160, 
     66,   0,   0,   3,   0,   0, 
     15, 128,   2,   0, 228, 176, 
      1,   8, 228, 160,  66,   0, 
      0,   3,   1,   0,  15, 128, 
      1,   0, 228, 176,   0,   8, 
    228, 160,   5,   0,   0,   3, 
      0,   0,   8, 128,   0,   0, 
    170, 128,   0,   0, 170, 128, 
      4,   0,   0,   4,   0,   0, 
      8, 128,   0,   0,  85, 128, 
      0,   0,  85, 128,   0,   0, 
    255, 128,   7,   0,   0,   2, 
      0,   0,   8, 128,   0,   0, 
    255, 128,   6,   0,   0,   2, 
      0,   0,   8, 128,   0,   0, 
    255, 128,   5,   0,   0,   3, 
      1,   0,   8, 128,   1,   0, 
    170, 128,   1,   0, 170, 128, 
      4,   0,   0,   4,   1,   0, 
      8, 128,   1,   0,  85, 128, 
      1,   0,  85, 128,   1,   0, 
    255, 128,   7,   0,   0,   2, 
      1,   0,   8, 128,   1,   0, 
    255, 128,   6,   0,   0,   2, 
      1,   0,   8, 128,   1,   0, 
    255, 128,   2,   0,   0,   3, 
      0,   0,  15, 128,   0,   0, 
    228, 129,   1,   0, 228, 128, 
      5,   0,   0,   3,   0,   0, 
      4, 128,   0,   0, 170, 128, 
      0,   0, 170, 128,   4,   0, 
      0,   4,   0,   0,   2, 128, 
      0,   0,  85, 128,   0,   0, 
     85, 128,   0,   0, 170, 128, 
      4,   0,   0,   4,   0,   0, 
      2, 128,   0,   0, 255, 128, 
      0,   0, 255, 129,   0,   0, 
     85, 128,   5,   0,   0,   3, 
      0,   0,   4, 128,   0,   0, 
    255, 128,   0,   0, 255, 128, 
      7,   0,   0,   2,   0,   0, 
      8, 128,   0,   0,  85, 128, 
      6,   0,   0,   2,   0,   0, 
      8, 128,   0,   0, 255, 128, 
     88,   0,   0,   4,   0,   0, 
      2, 128,   0,   0,  85, 129, 
      0,   0,   0, 160,   0,   0, 
    255, 128,   5,   0,   0,   3, 
      0,   0,   2, 128,   0,   0, 
     85, 128,   0,   0,  85, 128, 
      4,   0,   0,   4,   0,   0, 
      8, 128,   1,   0, 255, 128, 
      0,   0,  85, 160,   0,   0, 
    170, 160,   4,   0,   0,   4, 
      1,   0,   1, 128,   1,   0, 
    255, 128,   0,   0, 255, 160, 
      0,   0, 170, 160,   5,   0, 
      0,   3,   1,   0,   1, 128, 
      1,   0,   0, 128,   1,   0, 
      0, 128,   6,   0,   0,   2, 
      1,   0,   1, 128,   1,   0, 
      0, 128,   5,   0,   0,   3, 
      0,   0,   8, 128,   0,   0, 
    255, 128,   0,   0, 255, 128, 
      6,   0,   0,   2,   0,   0, 
      8, 128,   0,   0, 255, 128, 
      5,   0,   0,   3,   0,   0, 
      4, 128,   0,   0, 255, 128, 
      0,   0, 170, 128,   4,   0, 
      0,   4,   0,   0,   1, 128, 
      0,   0,   0, 128,   0,   0, 
      0, 128,   0,   0, 170, 128, 
      4,   0,   0,   4,   0,   0, 
      1, 128,   0,   0,  85, 128, 
      1,   0,   0, 128,   0,   0, 
      0, 128,   7,   0,   0,   2, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   6,   0,   0,   2, 
      0,   0,   1, 128,   0,   0, 
      0, 128,   1,   0,   0,   2, 
      0,   0,   6, 128,   1,   0, 
     85, 160,   1,   0,   0,   2, 
      0,   0,   8, 128,   1,   0, 
    255, 160,   1,   0,   0,   2, 
      0,   8,  15, 128,   0,   0, 
    228, 128, 255, 255,   0,   0, 
     83,  72,  68,  82,  64,   3, 
      0,   0,  64,   0,   0,   0, 
    208,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      0,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      1,   0,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   1,   0, 
      0,   0,  85,  85,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   2,   0,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   3,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   2,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      3,   0,   0,   0,  70, 126, 
     16,   0,   1,   0,   0,   0, 
      0,  96,  16,   0,   1,   0, 
      0,   0,  15,   0,   0,   7, 
    130,   0,  16,   0,   0,   0, 
      0,   0, 150,   5,  16,   0, 
      0,   0,   0,   0, 150,   5, 
     16,   0,   0,   0,   0,   0, 
     75,   0,   0,   5, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      2,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  15,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0, 150,   5,  16,   0, 
      1,   0,   0,   0, 150,   5, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   8, 114,   0, 
     16,   0,   0,   0,   0,   0, 
    150,   4,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
    150,   4,  16,   0,   1,   0, 
      0,   0,  75,   0,   0,   5, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   8, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  15,  50,   0, 
     16,   0,   1,   0,   0,   0, 
      6,   0,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
    236,  81,  56,  61, 143, 194, 
    117,  60,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  50,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7,  50,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  10,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  14,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,   9,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     49,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  75,   0, 
      0,   5,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  14,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
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
      0,   0,  25,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     17,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
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
     69,  70,   8,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,   0,   1,   0,   0, 
    212,   0,   0,   0, 156,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    170,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 184,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 198,   0, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
     73, 110, 112, 117, 116,  83, 
     97, 109, 112, 108, 101, 114, 
     48,   0,  73, 110, 112, 117, 
    116,  83,  97, 109, 112, 108, 
    101, 114,  49,   0,  73, 110, 
    112, 117, 116,  84, 101, 120, 
    116, 117, 114, 101,  48,   0, 
     73, 110, 112, 117, 116,  84, 
    101, 120, 116, 117, 114, 101, 
     49,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  57,  46,  50,  57,  46, 
     57,  53,  50,  46,  51,  49, 
     49,  49,   0, 171, 171, 171, 
     73,  83,  71,  78, 140,   0, 
      0,   0,   4,   0,   0,   0, 
      8,   0,   0,   0, 104,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 116,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0, 131,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   3,   0,   0, 131,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   3,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  83,  67, 
     69,  78,  69,  95,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0,  84,  69,  88,  67,  79, 
     79,  82,  68,   0,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     97, 114, 103, 101, 116,   0, 
    171, 171
};