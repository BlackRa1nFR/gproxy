

#define GCHeader	GCRef nextgc; uint8_t marked; uint8_t gct
struct GCRef {
	uint32_t gcptr32;
};
typedef struct GCHead {
	GCHeader;
} GCHead;
typedef std::uint32_t MSize;
typedef struct GCudata {
	GCHeader;
	uint8_t udtype;	/* Userdata type. */
	uint8_t unused2;
	GCRef env;		/* Should be at same offset in GCfunc. */
	MSize len;		/* Size of payload. */
	GCRef metatable;	/* Must be at same offset in GCtab. */
	uint32_t align1;	/* To force 8 byte alignment of the payload. */
} GCudata;

typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);
typedef void *MRef;

typedef struct GCState {
	MSize total;		/* Memory currently allocated. */
	MSize threshold;	/* Memory threshold. */
	uint8_t currentwhite;	/* Current white color. */
	uint8_t state;	/* GC state. */
	uint8_t nocdatafin;	/* No cdata finalizer called. */
	uint8_t unused2;
	MSize sweepstr;	/* Sweep position in string table. */
	GCRef root;		/* List of all collectable objects. */
	MRef sweep;		/* Sweep position in root list. */
	GCRef gray;		/* List of gray objects. */
	GCRef grayagain;	/* List of objects for atomic traversal. */
	GCRef weak;		/* List of weak tables (to be cleared). */
	GCRef mmudata;	/* List of userdata (to be finalized). */
	MSize stepmul;	/* Incremental GC step granularity. */
	MSize debt;		/* Debt (how much GC is behind schedule). */
	MSize estimate;	/* Estimate of memory actually in use. */
	MSize pause;		/* Pause between successive GC cycles. */
} GCState;

#define LJ_ENDIAN_LOHI(lo, hi) lo hi
#define LJ_ALIGN(n)	__declspec(align(n))
typedef double lua_Number;
typedef union {
	int32_t ftsz;		/* Frame type and size of previous frame. */
	MRef pcr;		/* Overlaps PC for Lua frames. */
} FrameLink;
typedef LJ_ALIGN(8) union TValue {
	uint64_t u64;		/* 64 bit pattern overlaps number. */
	lua_Number n;		/* Number object overlaps split tag/value object. */
	struct {
		LJ_ENDIAN_LOHI(
			union {
			GCRef gcr;	/* GCobj reference (if any). */
			int32_t i;	/* Integer value. */
		};
		, uint32_t it;	/* Internal object tag. Must overlap MSW of number. */
		)
	};
	struct {
		LJ_ENDIAN_LOHI(
			GCRef func;	/* Function for next frame (or dummy L). */
		, FrameLink tp;	/* Link to previous frame. */
		)
	} fr;
	struct {
		LJ_ENDIAN_LOHI(
			uint32_t lo;	/* Lower 32 bits of number. */
		, uint32_t hi;	/* Upper 32 bits of number. */
		)
	} u32;
} TValue;

typedef struct SBuf {
	char *buf;		/* String buffer base. */
	MSize n;		/* String buffer length. */
	MSize sz;		/* String buffer size. */
} SBuf;
typedef struct Node {
	TValue val;		/* Value object. Must be first field. */
	TValue key;		/* Key object. */
	MRef next;		/* Hash chain. */
	MRef freetop;		/* Top of free elements (stored in t->node[0]). */
} Node;
typedef struct GCstr {
	GCHeader;
	uint8_t reserved;	/* Used by lexer for fast lookup of reserved words. */
	uint8_t unused;
	MSize hash;		/* Hash of string. */
	MSize len;		/* Size of string. */
} GCstr;

struct global_State {
	GCRef *strhash;	/* String hash table (hash chain anchors). */
	MSize strmask;	/* String hash mask (size of hash table - 1). */
	MSize strnum;		/* Number of strings in hash table. */
	lua_Alloc allocf;	/* Memory allocator. */
	void *allocd;		/* Memory allocator data. */
	GCState gc;		/* Garbage collector. */
	SBuf tmpbuf;		/* Temporary buffer for string concatenation. */
	Node nilnode;		/* Fallback 1-element hash part (nil key and value). */
	GCstr strempty;	/* Empty string. */
	uint8_t stremptyz;	/* Zero terminator of empty string. */
	uint8_t hookmask;	/* Hook mask. */
	uint8_t dispatchmode;	/* Dispatch mode. */
	uint8_t vmevmask;	/* VM event mask. */
	GCRef mainthref;	/* Link to main thread. */
};/* Tagged value. */


struct lua_State {
	GCHeader;
	uint8_t dummy_ffid;
	uint8_t status;
	global_State *glref;
	GCRef gclist;		/* GC chain. */
	TValue *base;		/* Base of currently executing function. */
	TValue *top;		/* First free slot in the stack. */
};
#define setgcref(r, gc)	((r).gcptr32 = (uint32_t)(uintptr_t)&(gc)->gch)
#define setitype(o, i)		((o)->it = (i))
/* GCobj reference */
typedef union GCobj {
	GCHead gch;
	lua_State th;
	GCudata ud;
} GCobj;
static void setgcV(lua_State *L, TValue *o, GCobj *v, uint32_t itype)
{
	setgcref(o->gcr, v); setitype(o, itype);
}
#define obj2gco(v)	((GCobj *)(v))
#define define_setV(name, type, tag) \
static void name(lua_State *L, TValue *o, type *v) \
{ \
  setgcV(L, o, obj2gco(v), tag); \
}
#define LJ_TTHREAD		(~6u)
#define LJ_TUDATA		(~12u)
define_setV(setthreadV, lua_State, LJ_TTHREAD)
define_setV(setudataV, GCudata, LJ_TUDATA)
#define gcref(r)	((GCobj *)(uintptr_t)(r).gcptr32)
#define mainthread(g)	(&gcref(g->mainthref)->th)
#define setgcrefr(r, v)	((r).gcptr32 = (v).gcptr32)