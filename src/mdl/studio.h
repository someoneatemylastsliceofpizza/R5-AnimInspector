#pragma once

#include <vector>
#include <define.h>
#include <iostream>

#define STUDIO_ANIM_RAWROT  	0x20

#define R5_ANIM_RAWPOS		0x01
#define R5_ANIM_RAWROT		0x02
#define R5_ANIM_ANIMPOS		0x04
#define R5_ANIM_ANIMROT		0x08
#define R5_ANIM_RAWSCALE 	0x40
#define R5_ANIM_ANIMSCALE	0x80

#define STUDIO_ANIMPTR_Z 0x01
#define STUDIO_ANIMPTR_Y 0x02
#define STUDIO_ANIMPTR_X 0x04


namespace pt2 {
	struct mstudioanimsections_t
	{
		int animblock;
		int animindex;
	};
	struct mstudio_rle_anim_t
	{
		unsigned char bone;
		char flags;
		short nextoffset;
	};

	struct mstudiohitboxset_t
	{
		int sznameindex;
		int numhitboxes;
		int hitboxindex;
	};

	struct mstudioposeparamdesc_t
	{
		int	sznameindex;
		int flags;
		float start;
		float end;
		float loop;
	};

	struct mstudioiklink_t
	{
		int bone;
		Vector3	kneeDir;
		Vector3	unused0;
	};

	struct mstudiolinearbone_t
	{
		int numbones;
		int flagsindex;
		int	parentindex;
		int	posindex;
		int quatindex;
		int rotindex;
		int posetoboneindex;
		int	posscaleindex;
		int	rotscaleindex;
		int	qalignmentindex;
		int unused[6];
	};

	struct mstudioikchain_t
	{
		int sznameindex;
		int linktype;
		int numlinks;
		int linkindex;
	};

	struct mstudioautolayer_t
	{
		short iSequence;
		short iPose;
		int flags;
		float start;
		float peak;
		float tail;
		float end;
	};

	struct mstudiobone_t
	{
		int sznameindex;
		int parent;
		int bonecontroller[6];
		Vector3 pos;
		Quaternion quat;
		RadianEuler rot;
		Vector3 posscale;
		Vector3 rotscale;
		Matrix3x4_t poseToBone;
		Quaternion qAlignment;
		int flags;
		int proctype;
		int procindex;
		int physicsbone;
		int surfacepropidx;
		int contents;
		int surfacepropLookup;
		int unused[7];
	};

	struct studioanimvalue_ptr_t {
		short x;
		short y;
		short z;

		inline bool IsAllZero()
		{
			if (x == 0 && y == 0 && z == 0)
				return true;
			else
				return false;
		}

		inline short FirstThatIsNonZero() {
			if (x != 0) return x;
			if (y != 0) return y;
			if (z != 0) return z;
			return 0;
		}
	};

	struct studiohdr_t
	{
		int id;
		int version;
		long checksum;
		char name[64];
		int length;
		Vector3 eyeposition;
		Vector3 illumposition;
		Vector3 hull_min;
		Vector3 hull_max;
		Vector3 view_bbmin;
		Vector3 view_bbmax;
		int flags;
		int numbones;
		int boneindex;
		int numbonecontrollers;
		int bonecontrollerindex;
		int numhitboxsets;
		int hitboxsetindex;
		int numlocalanim;
		int localanimindex;
		int numlocalseq;
		int localseqindex;
		int activitylistversion;
		int eventsindexed;
		int numtextures;
		int textureindex;
		int numcdtextures;
		int cdtextureindex;
		int numskinref;
		int numskinfamilies;
		int skinindex;
		int numbodyparts;
		int bodypartindex;
		int numlocalattachments;
		int localattachmentindex;
		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;
		int numflexdesc;
		int flexdescindex;
		int numflexcontrollers;
		int flexcontrollerindex;
		int numflexrules;
		int flexruleindex;
		int numikchains;
		int ikchainindex;
		int nummouths;
		int mouthindex;
		int numlocalposeparameters;
		int localposeparamindex;
		int surfacepropindex;
		int keyvalueindex;
		int keyvaluesize;
		int numlocalikautoplaylocks;
		int localikautoplaylockindex;
		float mass;
		int contents;
		int numincludemodels;
		int includemodelindex;
		int virtualModel;
		int szanimblocknameindex;
		int numanimblocks;
		int animblockindex;
		int animblockModel;
		int bonetablebynameindex;
		int pVertexBase;
		int pIndexBase;
		byte constdirectionallightdot;
		byte rootLOD;
		byte numAllowedRootLODs;
		byte unused[1];
		int unused4;
		int numflexcontrollerui;
		int flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		mutable int surfacepropLookup;
		int studiohdr2index;
		int unused2[1];

		int numsrcbonetransform;
		int srcbonetransformindex;
		int	illumpositionattachmentindex;
		float flMaxEyeDeflection;
		int linearboneindex;
		int sznameindex;
		int m_nBoneFlexDriverCount;
		int m_nBoneFlexDriverIndex;
		int reserved[56];
	};


	struct mstudioseqdesc_t
	{
		int baseptr;
		int	sznameindex;
		int szactivitynameindex;
		int flags;
		int activity;
		int actweight;
		int numevents;
		int eventindex;
		Vector3 bbmin;
		Vector3 bbmax;
		int numblends;
		int animindexindex;
		int movementindex;
		int groupsize[2];
		int paramindex[2];
		float paramstart[2];
		float paramend[2];
		int paramparent;
		float fadeintime;
		float fadeouttime;
		int localentrynode;
		int localexitnode;
		int nodeflags;
		float entryphase;
		float exitphase;
		float lastframe;
		int nextseq;
		int pose;
		int numikrules;
		int numautolayers;
		int autolayerindex;
		int weightlistindex;
		int posekeyindex;
		int numiklocks;
		int iklockindex;
		int	keyvalueindex;
		int keyvaluesize;
		int cycleposeindex;
		int activitymodifierindex;
		int numactivitymodifiers;
		int unused[5];
	};

	struct mstudioevent_t
	{
		float cycle;
		int event;
		int type;
		char options[64];
		int szeventindex;

	};

	struct mstudioanimdesc_t
	{
		int baseptr;
		int sznameindex;
		float fps;
		int flags;
		int numframes;
		int	nummovements;
		int movementindex;
		int ikrulezeroframeindex;
		int unused1[5];
		int animblock;
		int animindex;
		int numikrules;
		int ikruleindex;
		int animblockikruleindex;
		int numlocalhierarchy;
		int localhierarchyindex;
		int sectionindex;
		int sectionframes;
		short zeroframespan;
		short zeroframecount;
		int zeroframeindex;
		float zeroframestalltime;
	};
}

namespace r5{
	namespace v10 {
		union mstudioanimvalue_t {
			struct {
				unsigned char	valid;
				unsigned char	total;
			} meta;
			short		value;
		};

		struct mstudiotexture_t
		{
			int sznameindex;
			__int64 guid;
		};

		struct mstudiobodyparts_t
		{
			short sznameindex;
			short modelindex;
			int base;
			int nummodels;
			int meshindex;
		};

		struct mstudioattachment_t
		{
			int sznameindex;
			int flags;
			int localbone; 
			Matrix3x4_t localmatrix;
		};

		struct mstudioanim_valueptr_t
		{
			short offset : 13;
			short flags : 3;
			unsigned char axisIdx1;
			unsigned char axisIdx2;
		};

		struct mstudio_rle_anim_t
		{
			short size : 13;
			short flags : 3;
		};

		struct boneflagarray {
			char flag;
		};

		struct studioanimvalue_ptr_t {
			short offset : 13;
			short flags : 3;
			char idx1;
			char idx2;
		};

		struct mstudiobone_t
		{
			int sznameindex;
			int parent;
			int bonecontroller[6];
			Vector3 pos;
			Quaternion quat;
			RadianEuler rot;
			Vector3 scale;
			Matrix3x4_t poseToBone;
			Quaternion qAlignment;
			int flags;
			int proctype;
			int procindex;
			int physicsbone;
			int surfacepropidx;
			int contents;
			int surfacepropLookup;
			int unk;
			int unkid;
		};

		struct mstudiohitboxset_t
		{
			int sznameindex;
			int numhitboxes;
			int hitboxindex;
		};

		struct mstudiomovement_t {
			int endframe;
			int motionflags;
			float v_start;
			float v_end;
			float angle;
			Vector3 movement;
			Vector3 position;
		};

		struct mstudioframemovement_t
		{
			float scale[4];
			int sectionFrames;
		};

		struct mstudiomodel_t
		{
			char name[64];
			int unkindex2;
			int type;
			float boundingradius;
			int nummeshes;
			int meshindex;
			int numvertices;
			int vertexindex;
			int tangentsindex;
			int numattachments;
			int attachmentindex;
			int deprecated_numeyeballs;
			int deprecated_eyeballindex;
			int pad[4];
			int colorindex; 
			int uv2index;
		};

		struct mstudioikrule_t
		{
			int index;
			int type;
			int chain;
			int bone;
			int slot;
			float height;
			float radius;
			float floor;
			Vector3 pos;
			Quaternion q;

			float scale[6];
			int sectionFrames;

			int compressedikerrorindex;
			int iStart;
			int ikerrorindex;
			float start;
			float peak;
			float tail;
			float end;
			float contact;
			float drop;
			float top;
			int szattachmentindex;
			float endHeight;
		};

		struct mstudioanimdesc_t
		{
			int baseptr;
			int sznameindex;
			float fps;
			int flags;
			int numframes;
			int nummovements;
			int movementindex;
			int framemovementindex;
			int animindex;
			int numikrules;
			int ikruleindex;
			int sectionindex;
			int sectionframes;
		};

		struct mstudioevent_t
		{
			float cycle;
			int event;
			int type;
			char options[256];
			int szeventindex;
		};

		struct mstudioautolayer_t
		{
			uint64_t guidSequence;
			short iSequence;
			short iPose;
			int flags;
			float start;
			float peak;
			float tail;
			float end;
		};

		struct mstudioactivitymodifier_t
		{
			int sznameindex;
			bool negate;
		};

		struct mstudioseqdesc_t
		{
			int baseptr;
			int	szlabelindex;
			int szactivitynameindex;
			int flags;
			int activity;
			int actweight;
			int numevents;
			int eventindex;
			Vector3 bbmin;
			Vector3 bbmax;
			int numblends;
			int animindexindex;
			int movementindex;
			int groupsize[2];
			int paramindex[2];
			float paramstart[2];
			float paramend[2];
			int paramparent;
			float fadeintime;
			float fadeouttime;
			int localentrynode;
			int localexitnode;
			int nodeflags;
			float entryphase;
			float exitphase;
			float lastframe;
			int nextseq;
			int pose;
			int numikrules;
			int numautolayers;
			int autolayerindex;
			int weightlistindex;
			int posekeyindex;
			int numiklocks;
			int iklockindex;
			int keyvalueindex;
			int keyvaluesize;
			int cycleposeindex;
			int activitymodifierindex;
			int numactivitymodifiers;
			int ikResetMask;
			int unk1;
			int unkOffset;
			int unkCount;
		};

		struct mstudioiklink_t
		{
			int bone;
			Vector3	kneeDir;
		};

		struct mstudioikchain_t
		{
			int sznameindex;
			int linktype;
			int numlinks;
			int linkindex;
			float unk;
		};

		struct mstudioposeparamdesc_t
		{
			int sznameindex;
			int flags;
			float start;
			float end;
			float loop;
		};

		struct studiohdr_t
		{
			int id;
			int version;
			int checksum;
			int sznameindex;
			char name[64];
			int length;
			Vector3 eyeposition;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int flags;
			int numbones;
			int boneindex;
			int numbonecontrollers;
			int bonecontrollerindex;
			int numhitboxsets;
			int hitboxsetindex;
			int numlocalanim;
			int localanimindex;
			int numlocalseq;
			int	localseqindex;
			int activitylistversion;
			int materialtypesindex;
			int numtextures;
			int textureindex;
			int numcdtextures;
			int cdtextureindex;
			int numskinref;
			int numskinfamilies;
			int skinindex;
			int numbodyparts;
			int bodypartindex;
			int numlocalattachments;
			int localattachmentindex;
			int numlocalnodes;
			int localnodeindex;
			int localnodenameindex;
			int unkNodeCount;
			int nodeDataOffsetsOffset;
			int meshOffset;
			int deprecated_numflexcontrollers;
			int deprecated_flexcontrollerindex;
			int deprecated_numflexrules;
			int deprecated_flexruleindex;
			int numikchains;
			int ikchainindex;
			int uiPanelCount;
			int uiPanelOffset;
			int numlocalposeparameters;
			int localposeparamindex;
			int surfacepropindex;
			int keyvalueindex;
			int keyvaluesize;
			int numlocalikautoplaylocks;
			int localikautoplaylockindex;
			float mass;
			int contents;
			int numincludemodels;
			int includemodelindex;
			int virtualModel;
			int bonetablebynameindex;
			char constdirectionallightdot;
			char rootLOD;
			char numAllowedRootLODs;
			char unused;
			float defaultFadeDist;
			float gatherSize;
			int deprecated_numflexcontrollerui;
			int deprecated_flexcontrolleruiindex;
			float flVertAnimFixedPointScale;
			int surfacepropLookup;
			int sourceFilenameOffset;
			int numsrcbonetransform;
			int srcbonetransformindex;
			int	illumpositionattachmentindex;
			int linearboneindex;
			int procBoneCount;
			int procBoneTableOffset;
			int linearProcBoneOffset;
			int deprecated_m_nBoneFlexDriverCount;
			int deprecated_m_nBoneFlexDriverIndex;
			int deprecated_m_nPerTriAABBIndex;
			int deprecated_m_nPerTriAABBNodeCount;
			int deprecated_m_nPerTriAABBLeafCount;
			int deprecated_m_nPerTriAABBVertCount;
			int unkStringOffset;
			int vtxOffset;
			int vvdOffset;
			int vvcOffset;
			int phyOffset;
			int vtxSize;
			int vvdSize;
			int vvcSize;
			int phySize;
			int deprecated_unkOffset;
			int deprecated_unkCount;
			int boneFollowerCount;
			int boneFollowerOffset;
			Vector3 mins;
			Vector3 maxs;
			int unk3_v54[3];
			int bvhOffset;
			short unk4_v54[2];
			int vvwOffset;
			int vvwSize;
		};
	}

	namespace v121 {
		struct studiohdr_t
		{
			int id;
			int version;
			int checksum;
			int sznameindex ;
			char name[64];
			int length;
			Vector3 eyeposition;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int flags;
			int numbones; 
			int boneindex;
			int numbonecontrollers;
			int bonecontrollerindex;
			int numhitboxsets;
			int hitboxsetindex;
			int numlocalanim; 
			int localanimindex;
			int numlocalseq;
			int	localseqindex;
			int activitylistversion;
			int materialtypesindex;
			int numtextures;
			int textureindex;
			int numcdtextures;
			int cdtextureindex;
			int numskinref;
			int numskinfamilies;
			int skinindex;
			int numbodyparts;
			int bodypartindex;
			int numlocalattachments;
			int localattachmentindex;
			int numlocalnodes;
			int localnodeindex;
			int localnodenameindex;
			int numunknodes;
			int nodedataindexindex;
			int numikchains;
			int ikchainindex;
			int numruimeshes;
			int ruimeshindex;
			int numlocalposeparameters;
			int localposeparamindex;
			int surfacepropindex;
			int keyvalueindex;
			int keyvaluesize;
			int numlocalikautoplaylocks;
			int localikautoplaylockindex;
			float mass;
			int contents;
			int numincludemodels;
			int includemodelindex;
			unsigned int virtualModel;
			int bonetablebynameindex;
			int numVGMeshes;
			int vgMeshIndex;
			int boneStateIndex;
			int numBoneStates;
			int unk_v54_v121;
			int vgSize;
			short vgUnk;
			short vgLODCount;
			int vgNumUnknown;
			int vgHeaderIndex;
			int numVGHeaders;
			int vgLODIndex;
			int numVGLODs;
			float fadeDistance;
			float gathersize; 
			float flVertAnimFixedPointScale;
			int surfacepropLookup;
			int sourceFilenameOffset;
			int numsrcbonetransform;
			int srcbonetransformindex;
			int	illumpositionattachmentindex;
			int linearboneindex;
			int numprocbonesunk;
			int procbonearrayindex;
			int procbonemaskindex;
			int unkstringindex;
			int vtxindex;
			int vvdindex;
			int vvcindex; 
			int vphyindex;
			int vtxsize;
			int vvdsize;
			int vvcsize;
			int vphysize;
			int numbonefollowers;
			int bonefollowerindex;
			Vector3 mins;
			Vector3 maxs;
			int bvh4index;
			short unk4_v54[2];
			int vvwindex;
			int vvwsize;
		};


		struct mstudiomodel_t
		{
			char name[64];
			int unkindex2; 
			int type;
			float boundingradius;
			int nummeshes;
			int meshindex;
			int numvertices;
			int vertexindex;
			int tangentsindex; 
			int numattachments;
			int attachmentindex;
			int colorindex; 
			int uv2index;
		};

		struct mstudiobone_t
		{
			int sznameindex;
			int parent;
			int bonecontroller[6];
			Vector3 pos;
			Quaternion quat;
			RadianEuler rot;
			Vector3 scale;
			Matrix3x4_t poseToBone;
			Quaternion qAlignment;
			int flags;
			int proctype;
			int procindex;
			int physicsbone;
			int surfacepropidx; 
			int contents;
			int surfacepropLookup; 
			byte unkid; 
			byte unk1[3];
		};

	}
	namespace v13 {

		struct studiohdr_t
		{
			int id;
			int version;
			int checksum;
			int sznameindex;
			char name[64];
			int length;
			Vector3 eyeposition;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			int flags;
			int numbones;
			int boneindex;
			int numbonecontrollers;
			int bonecontrollerindex;
			int numhitboxsets;
			int hitboxsetindex;
			int numlocalanim;
			int localanimindex;
			int numlocalseq;
			int	localseqindex;
			int activitylistversion;
			int materialtypesindex;
			int numtextures;
			int textureindex;
			int numcdtextures;
			int cdtextureindex;
			int numskinref;
			int numskinfamilies;
			int skinindex;
			int numbodyparts;
			int bodypartindex;
			int numlocalattachments;
			int localattachmentindex;
			int numlocalnodes;
			int localnodeindex;
			int localnodenameindex;
			int numunknodes;
			int nodedataindexindex;
			int numikchains;
			int ikchainindex;
			int numruimeshes;
			int ruimeshindex;
			int numlocalposeparameters;
			int localposeparamindex;
			int surfacepropindex;
			int keyvalueindex;
			int keyvaluesize;
			int numlocalikautoplaylocks;
			int localikautoplaylockindex;
			float mass;
			int contents;
			int numincludemodels;
			int includemodelindex;
			unsigned int virtualModel;
			int bonetablebynameindex;
			int numVGMeshes;
			int vgMeshIndex;
			int boneStateIndex;
			int numBoneStates;
			int unk_v54_v121;
			int vgSize;
			short vgUnk;
			short vgLODCount;
			int vgNumUnknown;
			int vgHeaderIndex;
			int numVGHeaders;
			int vgLODIndex;
			int numVGLODs;
			float fadeDistance;
			float gathersize;
			float flVertAnimFixedPointScale;
			int surfacepropLookup;
			int unk_v54_v122;
			int sourceFilenameOffset;
			int numsrcbonetransform;
			int srcbonetransformindex;
			int	illumpositionattachmentindex;
			int linearboneindex;
			int numprocbonesunk;
			int procbonearrayindex;
			int procbonemaskindex;
			int unkstringindex;
			int vtxindex;
			int vvdindex;
			int vvcindex;
			int vphyindex;
			int vtxsize;
			int vvdsize;
			int vvcsize;
			int vphysize;
			int numbonefollowers;
			int bonefollowerindex;
			Vector3 mins;
			Vector3 maxs;
			int bvh4index;
			short unk4_v54[2];
			int vvwindex;
			int vvwsize;
			int unk1_v54_v13[3];
		};
	}

	namespace v16 {

		struct studiohdr_t
		{
			int flags ;
			int checksum;
			short sznameindex;
			char name[32];
			byte unk_v16;
			byte surfacepropLookup;
			float mass;
			int version;
			unsigned short hitboxsetindex;
			byte numhitboxsets;
			byte illumpositionattachmentindex;
			Vector3 illumposition;
			Vector3 hull_min;
			Vector3 hull_max;
			Vector3 view_bbmin;
			Vector3 view_bbmax;
			short numbones;
			unsigned short boneindex;
			unsigned short bonedataindex;
			short numlocalseq;
			unsigned short localseqindex;
			short unk_v54_v14[2];
			byte activitylistversion;
			byte numlocalattachments;
			unsigned short localattachmentindex;
			short numlocalnodes;
			unsigned short localnodenameindex;
			unsigned short nodedataindexindex;
			short numikchains;
			unsigned short ikchainindex;
			short numtextures;
			unsigned short textureindex;
			short numskinref;
			short numskinfamilies;
			unsigned short skinindex;
			short numbodyparts;
			unsigned short bodypartindex;
			short numruimeshes;
			unsigned short ruimeshindex;
			short numlocalposeparameters;
			unsigned short localposeparamindex;
			unsigned short surfacepropindex;
			unsigned short keyvalueindex;
			unsigned short numVGMeshes;
			short vgMeshIndex;
			short bonetablebynameindex;
			unsigned short boneStateIndex;
			short numBoneStates;
			unsigned short vgHeaderIndex;
			short numVGHeaders;
			unsigned short vgLODIndex;
			short numVGLODs;
			float fadeDistance;
			float gathersize;
			short numsrcbonetransform;
			unsigned short srcbonetransformindex;
			short sourceFilenameOffset;
			unsigned short linearboneindex;
			unsigned short numprocbones;
			unsigned short procbonetableindex;
			unsigned short linearprocboneindex;
			short numbonefollowers;
			unsigned short bonefollowerindex;
			unsigned short bvh4index;
			byte unk5_v16;
			byte unk6_v16;
			short unk7_v16;
			short unk8_v16;
			short unk9_v16;
		};

		struct mstudioposeparamdesc_t
		{
			unsigned short sznameindex;
			short flags;
			float start;
			float end;
			float loop;
		};
		
		struct mstudioattachment_t
		{
			short sznameindex;
			short localbone;
			int flags;
			Matrix3x4_t localmatrix;
		};

		struct mstudiomodel_t
		{
			short unkindex2;
			short nummeshes;
			short unk_v14;
			short unk1_v14;
			short meshindex;
		};
		struct mstudiobone_t
		{
			int contents;
			byte unk;
			byte surfacepropLookup;
			short surfacepropidx;
			short physicsbone;
			short sznameindex;
		};

		struct mstudioikchain_t
		{
			unsigned short sznameindex;
			short linktype;
			short numlinks;
			unsigned short linkindex;
			float unk;
		};

		struct mstudioseqdesc_t
		{
			short szlabelindex;
			short szactivitynameindex;
			int flags;
			short activity;
			short actweight;
			short numevents;
			short eventindex;
			Vector3 bbmin;
			Vector3 bbmax;
			short numblends;
			short animindexindex;
			short paramindex[2]; 
			float paramstart[2]; 
			float paramend[2];
			float fadeintime;
			float fadeouttime;
			short localentrynode;
			short localexitnode;
			short numikrules;
			short numautolayers;
			unsigned short autolayerindex;
			unsigned short weightlistindex;
			byte groupsize[2];
			unsigned short posekeyindex;
			short numiklocks;
			short iklockindex;
			unsigned short unk_5C;
			short cycleposeindex;
			short activitymodifierindex;
			short numactivitymodifiers;
			int ikResetMask;
			int unk1;
			unsigned short unkindex;
			short unkcount;
		};

		struct mstudiocompressedikerror_t
		{
			short sectionframes;
			float scale[6];
		};

		struct mstudioikrule_t
		{
			short index;
			short bone;
			byte type;
			byte slot;
			mstudiocompressedikerror_t compressedikerror;
			int compressedikerrorindex;
			short iStart;
			short ikerrorindex;
			float start;
			float peak;
			float tail;
			float end;
			float contact;
			float drop;
			float top;
			float endHeight;
			float height;
			float radius;
			float floor;
			Vector3 pos;
			Quaternion q;
		};

		struct mstudioanimdesc_t
		{
			float fps;
			int flags;
			int numframes;
			short sznameindex;
			unsigned short framemovementindex;
			int animindex;
			short numikrules;
			unsigned short ikruleindex;
			__int64 unk2;
			uint16_t unk1;
			short sectionindex;
			short sectionstallframes;
			short sectionframes;
		};

		struct mstudioactivitymodifier_t
		{
			short sznameindex;
			bool negate;
		};

		struct mstudioevent_t
		{
			float cycle;
			int	event;
			int type;
			int unk;
			short szoptionsindex;
			short szeventindex;
		};

		struct linearboneindex_t
		{
			short numbones;
			short flagsindex;
			short parentindex;
			short posindex;
			short quatindex;
			short rotindex;
			short posetoboneindex;
		};


	}
}

namespace temp {

	struct ikchain_t
	{
		std::string name;
		std::string bone;
		float unk;
	};

	struct raw_bone_t {
		int name_index;
		int pad[45];
	};

	struct event_t {
		int frame;
		std::string options;
		std::string eventName;

		event_t(int frame, std::string options, std::string eventName)
			: frame(frame), options(options), eventName(eventName) {
		}
	};
}