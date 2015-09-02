// Two Phase Solver for the rubiks cube.
// This code is a manual translation of Herbet Kociembas java version to C.
// (http://kociemba.org/cube.htm)
// The translation was so straight-forward, it is obvious that the java version
// itself was a port from an original C/C++ implementation.


int Math_max(int a, int b)
{
    return a>b ? a:b;
}
int Math_min(int a, int b)
{
    return a<b ? a:b;
}


// --- Color.java
#define U 0
#define R 1
#define F 2
#define D 3
#define L 4
#define B 5

// --- Corner.java
#define URF 0
#define UFL 1
#define ULB 2
#define UBR 3
#define DFR 4
#define DLF 5
#define DBL 6
#define DRB 7

// --- Edge.java
#define UR 0
#define UF 1
#define UL 2
#define UB 3
#define DR 4
#define DF 5
#define DL 6
#define DB 7
#define FR 8
#define FL 9
#define BL 10
#define BR 11

// --- Facelet.java
/**
 * <pre>
 * The names of the facelet positions of the cube
 *             |************|
 *             |*U1**U2**U3*|
 *             |************|
 *             |*U4**U5**U6*|
 *             |************|
 *             |*U7**U8**U9*|
 *             |************|
 * ************|************|************|************|
 * *L1**L2**L3*|*F1**F2**F3*|*R1**R2**F3*|*B1**B2**B3*|
 * ************|************|************|************|
 * *L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*|
 * ************|************|************|************|
 * *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*|
 * ************|************|************|************|
 *             |************|
 *             |*D1**D2**D3*|
 *             |************|
 *             |*D4**D5**D6*|
 *             |************|
 *             |*D7**D8**D9*|
 *             |************|
 * </pre>
 *
 *A cube definition list "UBL..." means for example: In position U1 we have the U-color, in position U2 we have the
 * B-color, in position U3 we have the L color etc. according to the order U1, U2, U3, U4, U5, U6, U7, U8, U9, R1, R2,
 * R3, R4, R5, R6, R7, R8, R9, F1, F2, F3, F4, F5, F6, F7, F8, F9, D1, D2, D3, D4, D5, D6, D7, D8, D9, L1, L2, L3, L4,
 * L5, L6, L7, L8, L9, B1, B2, B3, B4, B5, B6, B7, B8, B9 of the enum constants.
 */
#define U1 0
#define U2 1
#define U3 2
#define U4 3
#define U5 4
#define U6 5
#define U7 6
#define U8 7
#define U9 8
#define R1 9
#define R2 10
#define R3 11
#define R4 12
#define R5 13
#define R6 14
#define R7 15
#define R8 16
#define R9 17
#define F1 18
#define F2 19
#define F3 20
#define F4 21
#define F5 22
#define F6 23
#define F7 24
#define F8 25
#define F9 26
#define D1 27
#define D2 28
#define D3 29
#define D4 30
#define D5 31
#define D6 32
#define D7 33
#define D8 34
#define D9 35
#define L1 36
#define L2 37
#define L3 38
#define L4 39
#define L5 40
#define L6 41
#define L7 42
#define L8 43
#define L9 44
#define B1 45
#define B2 46
#define B3 47
#define B4 48
#define B5 49
#define B6 50
#define B7 51
#define B8 52
#define B9 53




// --- FaceCube.java

//Cube on the facelet level
struct _FaceCube
{
    int f[54];
};
typedef struct _FaceCube *FaceCube;

 

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Map the corner positions to facelet positions. cornerFacelet[URF.ordinal()][0] e.g. gives the position of the
	// facelet in the URF corner position, which defines the orientation.<br>
	// cornerFacelet[URF.ordinal()][1] and cornerFacelet[URF.ordinal()][2] give the position of the other two facelets
	// of the URF corner (clockwise).
  int cornerFacelet[8][3] = { { U9, R1, F3 }, { U7, F1, L3 }, { U1, L1, B3 }, { U3, B1, R3 },
			{ D3, F9, R7 }, { D1, L9, F7 }, { D7, B9, L7 }, { D9, R9, B7 } };
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Map the edge positions to facelet positions. edgeFacelet[UR.ordinal()][0] e.g. gives the position of the facelet in
	// the UR edge position, which defines the orientation.<br>
	// edgeFacelet[UR.ordinal()][1] gives the position of the other facelet
  int edgeFacelet[12][2] = { { U6, R2 }, { U8, F2 }, { U4, L2 }, { U2, B2 }, { D6, R8 }, { D2, F8 },
		{ D4, L8 }, { D8, B8 }, { F6, R4 }, { F4, L6 }, { B6, L4 }, { B4, R6 } };
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Map the corner positions to facelet colors.
  int cornerColor[8][3] = { { U, R, F }, { U, F, L }, { U, L, B }, { U, B, R }, { D, F, R }, { D, L, F },
		{ D, B, L }, { D, R, B } };
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Map the edge positions to facelet colors.
  int edgeColor[12][2] = { { U, R }, { U, F }, { U, L }, { U, B }, { D, R }, { D, F }, { D, L }, { D, B },
		{ F, R }, { F, L }, { B, L }, { B, R } };

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Construct a facelet cube from a string
FaceCube FaceCube_construct(FaceCube this, char* faceList) {
    int i,j;
    for (i=0; i<54; i++)
    {   this->f[i] = faceList[i];
    }
    return this;
}

FaceCube FaceCube_construct_nopar(FaceCube this)
{
   int i;
    for (i=0; i<54; i++)
    {   this->f[i] = i/9;
    }
    return this;
}

int FaceCube_verifyCounts(FaceCube this)
{
		int count[6] = { 0,0,0,0,0,0 };
      int i;
			for (i = 0; i < 54; i++)
      { int j = this->f[i];
        if (j>=0 && j<=5)
        {   count[j]++;
        }
      }
		for (i = 0; i < 6; i++)
			if (count[i] != 9)
      { return 1;
      }
     return 0;
}


//	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//	// Gives string representation of a facelet cube
//void FaceCube_toString(FaceCube this, char* buffer) {
//    int i;
//    for (i=0; i<54; i++)
//    { buffer[i] = Color_identifiers[this->f[i]];
//    }
//    buffer[54] = 0;
//}



// --- CubieCube.java

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Cube on the cubie level
struct _CubieCube {

	// corner permutation
  int cp[8];

	// corner orientation
  char co[8];

	// edge permutation
  int ep[12];

	// edge orientation
  char eo[12];
};
typedef struct _CubieCube *CubieCube;

 
	// ************************************** Moves on the cubie level ***************************************************

	int cpU[] = { UBR, URF, UFL, ULB, DFR, DLF, DBL, DRB };
  char coU[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int epU[] = { UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR };
	char eoU[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int cpR[] = { DFR, UFL, ULB, URF, DRB, DLF, DBL, UBR };
	char coR[] = { 2, 0, 0, 1, 1, 0, 0, 2 };
	int epR[] = { FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR };
	char eoR[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int cpF[] = { UFL, DLF, ULB, UBR, URF, DFR, DBL, DRB };
	char coF[] = { 1, 2, 0, 0, 2, 1, 0, 0 };
	int epF[] = { UR, FL, UL, UB, DR, FR, DL, DB, UF, DF, BL, BR };
	char eoF[] = { 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0 };

	int cpD[] = { URF, UFL, ULB, UBR, DLF, DBL, DRB, DFR };
	char coD[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int epD[] = { UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR };
	char eoD[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int cpL[] = { URF, ULB, DBL, UBR, DFR, UFL, DLF, DRB };
	char coL[] = { 0, 1, 2, 0, 0, 2, 1, 0 };
	int epL[] = { UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR };
	char eoL[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int cpB[] = { URF, UFL, UBR, DRB, DFR, DLF, ULB, DBL };
	char coB[] = { 0, 0, 1, 2, 0, 0, 2, 1 };
	int epB[] = { UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB };
	char eoB[] = { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1 };


  CubieCube CubieCube_construct_nopar(CubieCube this)
  {
    int i;
   	this->cp[0] = URF;
    this->cp[1] = UFL;
    this->cp[2] = ULB;
    this->cp[3] = UBR;
    this->cp[4] = DFR;
    this->cp[5] = DLF;
    this->cp[6] = DBL;
    this->cp[7] = DRB;
    for (i=0; i<8; i++)
    {   this->co[i] = 0;
    }
    this->ep[0] = UR;
    this->ep[1] = UF;
    this->ep[2] = UL;
    this->ep[3] = UB;
    this->ep[4] = DR;
    this->ep[5] = DF;
    this->ep[6] = DL;
    this->ep[7] = DB;
    this->ep[8] = FR;
    this->ep[9] = FL;
    this->ep[10] = BL;
    this->ep[11] = BR;
    for (i=0; i<12; i++)
    {   this->eo[i] = 0;
    }
    return this;
}
CubieCube CubieCube_construct(CubieCube this, int* cp, char* co, int* ep, char* eo)
{
    int i;
		for (i = 0; i < 8; i++) {
			this->cp[i] = cp[i];
			this->co[i] = co[i];
		}
		for (i = 0; i < 12; i++) {
			this->ep[i] = ep[i];
			this->eo[i] = eo[i];
		}
    return this;
}

	// constructr a FaceCube from a CubieCube
CubieCube CubieCube_constructFromFaceCube(CubieCube this, FaceCube facecube) {

		char ori;
		CubieCube_construct_nopar(this);

		int i,j;
		for (i = 0; i < 8; i++)
			this->cp[i] = URF;// invalidate corners
		for (i = 0; i < 12; i++)
			this->ep[i] = UR;// and edges
		int col1, col2;
		for (i=0; i<8; i++) {
			// get the colors of the cubie at corner i, starting with U/D
			for (ori = 0; ori < 3; ori++)
				if (facecube->f[cornerFacelet[i][ori]] == U || facecube->f[cornerFacelet[i][ori]] == D)
					break;
			col1 = facecube->f[cornerFacelet[i][(ori + 1) % 3]];
			col2 = facecube->f[cornerFacelet[i][(ori + 2) % 3]];

			for (j=0; j<8; j++) {
				if (col1 == cornerColor[j][1] && col2 == cornerColor[j][2]) {
					// in cornerposition i we have cornercubie j
					this->cp[i] = j;
					this->co[i] = (char) (ori % 3);
					break;
				}
			}
		}
		for (i=0; i<12; i++)
			for (j=0; j<12; j++) {
				if (facecube->f[edgeFacelet[i][0]] == edgeColor[j][0]
						&& facecube->f[edgeFacelet[i][1]] == edgeColor[j][1]) {
					this->ep[i] = j;
					this->eo[i] = 0;
					break;
				}
				if (facecube->f[edgeFacelet[i][0]] == edgeColor[j][1]
						&& facecube->f[edgeFacelet[i][1]] == edgeColor[j][0]) {
					this->ep[i] = j;
					this->eo[i] = 1;
					break;
				}
			}
		return this;
	}





	// this CubieCube array represents the 6 basic cube moves
	struct _CubieCube _moveCube[6];
	CubieCube moveCube[6];
	
void CubieCube_initstatic()
{
	moveCube[0] = CubieCube_construct(&_moveCube[0], cpU,coU,epU,eoU);
	moveCube[1] = CubieCube_construct(&_moveCube[1], cpR,coR,epR,eoR);
	moveCube[2] = CubieCube_construct(&_moveCube[2], cpF,coF,epF,eoF);
	moveCube[3] = CubieCube_construct(&_moveCube[3], cpD,coD,epD,eoD);
	moveCube[4] = CubieCube_construct(&_moveCube[4], cpL,coL,epL,eoL);
	moveCube[5] = CubieCube_construct(&_moveCube[5], cpB,coB,epB,eoB);
}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// n choose k
	static int Cnk(int n, int k) {
		int i, j, s;
		if (n < k)
			return 0;
		if (k > n / 2)
			k = n - k;
		for (s = 1, i = n, j = 1; i != n - k; i--, j++) {
			s *= i;
			s /= j;
		}
		return s;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void rotateLeft(int* arr, int l, int r)
	// Left rotation of all array elements between l and r
	{
    int i;
		int temp = arr[l];
		for (i = l; i < r; i++)
			arr[i] = arr[i + 1];
		arr[r] = temp;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void rotateRight(int* arr, int l, int r)
	// Right rotation of all array elements between l and r
	{
    int i;
		int temp = arr[r];
		for (i = r; i > l; i--)
			arr[i] = arr[i - 1];
		arr[l] = temp;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// create cube in facelet representation -> fill in the given fcRect structure)
	void CubieCube_toFaceCube(CubieCube this, FaceCube fcRet)	
	{
		int c;
		for (c=0; c<8; c++) {
			int i = c;
			int j = this->cp[i]; // cornercubie with index j is at
			// cornerposition with index i
			char ori = this->co[i];// Orientation of this cubie
      int n;
			for (n = 0; n < 3; n++)
				fcRet->f[cornerFacelet[i][(n + ori) % 3]] = cornerColor[j][n];
		}
    int e;
		for (e=0; e<12; e++) {
			int i = e;
			int j = this->ep[i];// edgecubie with index j is at edgeposition
			// with index i
			char ori = this->eo[i];// Orientation of this cubie
      int n;
			for (n = 0; n < 2; n++)
				fcRet->f[edgeFacelet[i][(n + ori) % 2]] = edgeColor[j][n];
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Multiply this CubieCube with another cubiecube b, restricted to the corners.<br>
	// Because we also describe reflections of the whole cube by permutations, we get a complication with the corners. The
	// orientations of mirrored corners are described by the numbers 3, 4 and 5. The composition of the orientations
	// cannot
	// be computed by addition modulo three in the cyclic group C3 any more. Instead the rules below give an addition in
	// the dihedral group D3 with 6 elements.<br>
	//
	// NOTE: Because we do not use symmetry reductions and hence no mirrored cubes in this simple implementation of the
	// Two-Phase-Algorithm, some code is not necessary here.
	//
	void CubieCube_cornerMultiply(CubieCube this, CubieCube b) {
		int cPerm[8] = { 0,0,0,0,0,0,0,0};
		char cOri[8] = { 0,0,0,0,0,0,0,0};
    int corn,c;
		for (corn=0; corn<8; corn++) {
			cPerm[corn] = this->cp[b->cp[corn]];

			char oriA = this->co[b->cp[corn]];
			char oriB = b->co[corn];
			char ori = 0;
			;
			if (oriA < 3 && oriB < 3) // if both cubes are regular cubes...
			{
				ori = (char) (oriA + oriB); // just do an addition modulo 3 here
				if (ori >= 3)
					ori -= 3; // the composition is a regular cube

				// +++++++++++++++++++++not used in this implementation +++++++++++++++++++++++++++++++++++
			} else if (oriA < 3 && oriB >= 3) // if cube b is in a mirrored
			// state...
			{
				ori = (char) (oriA + oriB);
				if (ori >= 6)
					ori -= 3; // the composition is a mirrored cube
			} else if (oriA >= 3 && oriB < 3) // if cube a is an a mirrored
			// state...
			{
				ori = (char) (oriA - oriB);
				if (ori < 3)
					ori += 3; // the composition is a mirrored cube
			} else if (oriA >= 3 && oriB >= 3) // if both cubes are in mirrored
			// states...
			{
				ori = (char) (oriA - oriB);
				if (ori < 0)
					ori += 3; // the composition is a regular cube
				// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			}
			cOri[corn] = ori;
		}
		for (c=0; c<8; c++) {
			this->cp[c] = cPerm[c];
			this->co[c] = cOri[c];
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Multiply this CubieCube with another cubiecube b, restricted to the edges.
	void CubieCube_edgeMultiply(CubieCube this, CubieCube b) {
		int ePerm[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
		char eOri[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
    int edge,e;
		for (edge=0; edge<12; edge++) {
			ePerm[edge] = this->ep[b->ep[edge]];
			eOri[edge] = (char) ((b->eo[edge] + this->eo[b->ep[edge]]) % 2);
		}
		for (e=0; e<12; e++) {
			this->ep[e] = ePerm[e];
			this->eo[e] = eOri[e];
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Multiply this CubieCube with another CubieCube b.
	void CubieCube_multiply(CubieCube this, CubieCube b) {
		CubieCube_cornerMultiply(this,b);
		// edgeMultiply(b);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Compute the inverse CubieCube
	void CubieCube_invCubieCube(CubieCube this, CubieCube c) {
    int edge,corn;
		for (edge=0; edge<12; edge++)
			c->ep[c->ep[edge]] = edge;
		for (edge=0; edge<12; edge++)
			c->eo[edge] = this->eo[c->ep[edge]];
		for (corn=0; corn<8; corn++)
			c->cp[this->cp[corn]] = corn;
		for (corn=0; corn<8; corn++) {
			char ori = this->co[c->cp[corn]];
			if (ori >= 3)// Just for completeness. We do not invert mirrored
				// cubes in the program.
				c->co[corn] = ori;
			else {// the standard case
				c->co[corn] = (char) -ori;
				if (c->co[corn] < 0)
					c->co[corn] += 3;
			}
		}
	}

	// ********************************************* Get and set coordinates *********************************************

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// return the twist of the 8 corners. 0 <= twist < 3^7
	short CubieCube_getTwist(CubieCube this) {
		short ret = 0;
    int i;
		for (i = URF; i < DRB; i++)
			ret = (short) (3 * ret + this->co[i]);
		return ret;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setTwist(CubieCube this, short twist) {
		int twistParity = 0;
    int i;
		for (i = DRB - 1; i >= URF; i--) {
			twistParity += this->co[i] = (char) (twist % 3);
			twist /= 3;
		}
		this->co[DRB] = (char) ((3 - twistParity % 3) % 3);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// return the flip of the 12 edges. 0<= flip < 2^11
	short CubieCube_getFlip(CubieCube this) {
		short ret = 0;
    int i;
		for (i = UR; i < BR; i++)
			ret = (short) (2 * ret + this->eo[i]);
		return ret;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setFlip(CubieCube this, short flip) {
		int flipParity = 0;
    int i;
		for (i = BR - 1; i >= UR; i--) {
			flipParity += this->eo[i] = (char) (flip % 2);
			flip /= 2;
		}
		this->eo[BR] = (char) ((2 - flipParity % 2) % 2);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Parity of the corner permutation
	short CubieCube_cornerParity(CubieCube this) {
		int s = 0;
    int i,j;
		for (i = DRB; i >= URF + 1; i--)
			for (j = i - 1; j >= URF; j--)
				if (this->cp[j] > this->cp[i])
					s++;
		return (short) (s % 2);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Parity of the edges permutation. Parity of corners and edges are the same if the cube is solvable.
	short CubieCube_edgeParity(CubieCube this) {
		int s = 0;
    int i,j;
		for (i = BR; i >= UR + 1; i--)
			for (j = i - 1; j >= UR; j--)
				if (this->ep[j] > this->ep[i])
					s++;
		return (short) (s % 2);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// permutation of the UD-slice edges FR,FL,BL and BR
	short CubieCube_getFRtoBR(CubieCube this) {
		int a = 0, x = 0;
		int edge4[4] = { 0,0,0,0 };
    int i,j;
		// compute the index a < (12 choose 4) and the permutation array perm.
		for (j = BR; j >= UR; j--)
			if (FR <= this->ep[j] && this->ep[j] <= BR) {
				a += Cnk(11 - j, x + 1);
				edge4[3 - x++] = this->ep[j];
			}

		int b = 0;
		for (j = 3; j > 0; j--)// compute the index b < 4! for the
		// permutation in perm
		{
			int k = 0;
			while (edge4[j] != j + 8) {
				rotateLeft(edge4, 0, j);
				k++;
			}
			b = (j + 1) * b + k;
		}
		return (short) (24 * a + b);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setFRtoBR(CubieCube this, short idx) {
		int x;
		int sliceEdge[4] = { FR, FL, BL, BR };
		int otherEdge[8] = { UR, UF, UL, UB, DR, DF, DL, DB };
		int b = idx % 24; // Permutation
		int a = idx / 24; // Combination
    int e,j;
		for (e=0; e<12; e++)
			this->ep[e] = DB;// Use UR to invalidate all edges

		for (j = 1; j < 4; j++)// generate permutation from index b
		{
			int k = b % (j + 1);
			b /= j + 1;
			while (k-- > 0)
				rotateRight(sliceEdge, 0, j);
		}

		x = 3;// generate combination and set slice edges
		for (j = UR; j <= BR; j++)
			if (a - Cnk(11 - j, x + 1) >= 0) {
				this->ep[j] = sliceEdge[3 - x];
				a -= Cnk(11 - j, x-- + 1);
			}
		x = 0; // set the remaining edges UR..DB
		for (j = UR; j <= BR; j++)
			if (this->ep[j] == DB)
				this->ep[j] = otherEdge[x++];

	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Permutation of all corners except DBL and DRB
	short CubieCube_getURFtoDLF(CubieCube this) {
		int a = 0, x = 0;
		int corner6[6] = { 0,0,0,0,0,0 };
    int j;
		// compute the index a < (8 choose 6) and the corner permutation.
		for (j = URF; j <= DRB; j++)
			if (this->cp[j] <= DLF) {
				a += Cnk(j, x + 1);
				corner6[x++] = this->cp[j];
			}

		int b = 0;
		for (j = 5; j > 0; j--)// compute the index b < 6! for the
		// permutation in corner6
		{
			int k = 0;
			while (corner6[j] != j) {
				rotateLeft(corner6, 0, j);
				k++;
			}
			b = (j + 1) * b + k;
		}
		return (short) (720 * a + b);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setURFtoDLF(CubieCube this, short idx) {
		int x;
	  int corner6[6] = { URF, UFL, ULB, UBR, DFR, DLF };
		int otherCorner[2] = { DBL, DRB };
		int b = idx % 720; // Permutation
		int a = idx / 720; // Combination
    int c;
		for (c=0; c<8; c++)
			this->cp[c] = DRB;// Use DRB to invalidate all corners

    int j;
		for (j = 1; j < 6; j++)// generate permutation from index b
		{
			int k = b % (j + 1);
			b /= j + 1;
			while (k-- > 0)
				rotateRight(corner6, 0, j);
		}
		x = 5;// generate combination and set corners
		for (j = DRB; j >= 0; j--)
			if (a - Cnk(j, x + 1) >= 0) {
				this->cp[j] = corner6[x];
				a -= Cnk(j, x-- + 1);
			}
		x = 0;
		for (j = URF; j <= DRB; j++)
			if (this->cp[j] == DRB)
				this->cp[j] = otherCorner[x++];
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Permutation of the six edges UR,UF,UL,UB,DR,DF.
	int CubieCube_getURtoDF(CubieCube this) {
		int a = 0, x = 0;
		int edge6[6] = { 0,0,0,0,0,0 };
		// compute the index a < (12 choose 6) and the edge permutation.
    int j;
		for (j = UR; j <= BR; j++)
			if (this->ep[j] <= DF) {
				a += Cnk(j, x + 1);
				edge6[x++] = this->ep[j];
			}

		int b = 0;
		for (j = 5; j > 0; j--)// compute the index b < 6! for the
		// permutation in edge6
		{
			int k = 0;
			while (edge6[j] != j) {
				rotateLeft(edge6, 0, j);
				k++;
			}
			b = (j + 1) * b + k;
		}
		return 720 * a + b;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setURtoDF(CubieCube this, int idx) {
		int x;
		int edge6[6] = { UR, UF, UL, UB, DR, DF };
		int otherEdge[6] = { DL, DB, FR, FL, BL, BR };
		int b = idx % 720; // Permutation
		int a = idx / 720; // Combination
    int e;
		for (e=0; e<12; e++)
			this->ep[e] = BR;// Use BR to invalidate all edges

    int j;
		for (j = 1; j < 6; j++)// generate permutation from index b
		{
			int k = b % (j + 1);
			b /= j + 1;
			while (k-- > 0)
				rotateRight(edge6, 0, j);
		}
		x = 5;// generate combination and set edges
		for (j = BR; j >= 0; j--)
			if (a - Cnk(j, x + 1) >= 0) {
				this->ep[j] = edge6[x];
				a -= Cnk(j, x-- + 1);
			}
		x = 0; // set the remaining edges DL..BR
		for (j = UR; j <= BR; j++)
			if (this->ep[j] == BR)
				this->ep[j] = otherEdge[x++];
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setURtoUL(CubieCube this, short idx) {
		int x;
		int edge3[3] = { UR, UF, UL };
		int b = idx % 6; // Permutation
		int a = idx / 6; // Combination
    int e;
		for (e=0; e<12; e++)
			this->ep[e] = BR;// Use BR to invalidate all edges
    int j;
		for (j = 1; j < 3; j++)// generate permutation from index b
		{
			int k = b % (j + 1);
			b /= j + 1;
			while (k-- > 0)
				rotateRight(edge3, 0, j);
		}
		x = 2;// generate combination and set edges
		for (j = BR; j >= 0; j--)
			if (a - Cnk(j, x + 1) >= 0) {
				this->ep[j] = edge3[x];
				a -= Cnk(j, x-- + 1);
			}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Permutation of the three edges UB,DR,DF
	short CubieCube_getUBtoDF(CubieCube this) {
		int a = 0, x = 0;
		int edge3[3] = { 0,0,0 };
		// compute the index a < (12 choose 3) and the edge permutation.
    int j;
		for (j = UR; j <= BR; j++)
			if (UB <= this->ep[j] && this->ep[j] <= DF) {
				a += Cnk(j, x + 1);
				edge3[x++] = this->ep[j];
			}

		int b = 0;
		for (j = 2; j > 0; j--)// compute the index b < 3! for the
		// permutation in edge3
		{
			int k = 0;
			while (edge3[j] != UB + j) {
				rotateLeft(edge3, 0, j);
				k++;
			}
			b = (j + 1) * b + k;
		}
		return (short) (6 * a + b);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Permutation of the three edges UR,UF,UL
	short CubieCube_getURtoUL(CubieCube this) {
		int a = 0, x = 0;
		int edge3[3] = { 0,0,0 };
		// compute the index a < (12 choose 3) and the edge permutation.
    int j;
		for (j = UR; j <= BR; j++)
			if (this->ep[j] <= UL) {
				a += Cnk(j, x + 1);
				edge3[x++] = this->ep[j];
			}

		int b = 0;
		for (j = 2; j > 0; j--)// compute the index b < 3! for the
		// permutation in edge3
		{
			int k = 0;
			while (edge3[j] != j) {
				rotateLeft(edge3, 0, j);
				k++;
			}
			b = (j + 1) * b + k;
		}
		return (short) (6 * a + b);
	}


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setUBtoDF(CubieCube this, short idx) {
		int x;
		int edge3[3] = { UB, DR, DF };
		int b = idx % 6; // Permutation
		int a = idx / 6; // Combination
    int e;
		for (e=0; e<12; e++)
			this->ep[e] = BR;// Use BR to invalidate all edges
    int j;
		for (j = 1; j < 3; j++)// generate permutation from index b
		{
			int k = b % (j + 1);
			b /= j + 1;
			while (k-- > 0)
				rotateRight(edge3, 0, j);
		}
		x = 2;// generate combination and set edges
		for (j = BR; j >= 0; j--)
			if (a - Cnk(j, x + 1) >= 0) {
				this->ep[j] = edge3[x];
				a -= Cnk(j, x-- + 1);
			}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int CubieCube_getURFtoDLB(CubieCube this) {
		int perm[8] = { 0,0,0,0,0,0,0,0 };
		int b = 0;
    int i;
		for (i = 0; i < 8; i++)
			perm[i] = this->cp[i];
    int j;
		for (j = 7; j > 0; j--)// compute the index b < 8! for the permutation in perm
		{
			int k = 0;
			while (perm[j] != j) {
				rotateLeft(perm, 0, j);
				k++;
			}
			b = (j + 1) * b + k;
		}
		return b;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setURFtoDLB(CubieCube this, int idx) {
		int perm[8] = { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB };
		int k;
    int j;
		for (j = 1; j < 8; j++) {
			k = idx % (j + 1);
			idx /= j + 1;
			while (k-- > 0)
				rotateRight(perm, 0, j);
		}
		int x = 7;// set corners
		for (j = 7; j >= 0; j--)
			this->cp[j] = perm[x--];
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	int CubieCube_getURtoBR(CubieCube this) {
		int perm[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
		int b = 0;
    int i;
		for (i = 0; i < 12; i++)
			perm[i] = this->ep[i];
    int j;
		for (j = 11; j > 0; j--)// compute the index b < 12! for the permutation in perm
		{
			int k = 0;
			while (perm[j] != j) {
				rotateLeft(perm, 0, j);
				k++;
			}
			b = (j + 1) * b + k;
		}
		return b;
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CubieCube_setURtoBR(CubieCube this, int idx) {
		int perm[12] = { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };
		int k;
    int j;
		for (j = 1; j < 12; j++) {
			k = idx % (j + 1);
			idx /= j + 1;
			while (k-- > 0)
				rotateRight(perm, 0, j);
		}
		int x = 11;// set edges
		for (j = 11; j >= 0; j--)
			this->ep[j] = perm[x--];
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Check a cubiecube for solvability. Return the error code.
	// 0: Cube is solvable
	// 2: Not all 12 edges exist exactly once
	// 3: Flip error: One edge has to be flipped
	// 4: Not all corners exist exactly once
	// 5: Twist error: One corner has to be twisted
	// 6: Parity error: Two corners or two edges have to be exchanged
	int CubieCube_verify(CubieCube this) {
		int sum = 0;
		int edgeCount[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
    int e;
		for (e=0; e<12; e++)
			edgeCount[this->ep[e]]++;
    int i;
		for (i = 0; i < 12; i++)
			if (edgeCount[i] != 1)
				return 2;

		for (i = 0; i < 12; i++)
			sum += this->eo[i];
		if (sum % 2 != 0)
			return 3;

		int cornerCount[8] = { 0,0,0,0,0,0,0,0 };
    int c;
		for (c=0; c<8; c++)
			cornerCount[this->cp[c]]++;
		for (i = 0; i < 8; i++)
			if (cornerCount[i] != 1)
				return 4;// missing corners

		sum = 0;
		for (i = 0; i < 8; i++)
			sum += this->co[i];
		if (sum % 3 != 0)
			return 5;// twisted corner

		if ((CubieCube_edgeParity(this) ^ CubieCube_cornerParity(this)) != 0)
			return 6;// parity error

		return 0;// cube ok
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Permutation of the six edges UR,UF,UL,UB,DR,DF
	int getURtoDF(short idx1, short idx2) {
    struct _CubieCube _a,_b;
		CubieCube a = CubieCube_construct_nopar(&_a);
		CubieCube b = CubieCube_construct_nopar(&_b);
		CubieCube_setURtoUL(a,idx1);
		CubieCube_setUBtoDF(b,idx2);
    int i;
		for (i = 0; i < 8; i++) {
			if (a->ep[i] != BR)
				if (b->ep[i] != BR)// collision
					return -1;
				else
					b->ep[i] = a->ep[i];
		}
		return CubieCube_getURtoDF(b);
	}


// --- CooordCube.java

	#define N_TWIST  2187  // 3^7 possible corner orientations
	#define N_FLIP  2048   // 2^11 possible edge flips
	#define N_SLICE1 495   // 12 choose 4 possible positions of FR,FL,BL,BR edges
	#define N_SLICE2  24   // 4! permutations of FR,FL,BL,BR edges in phase2
	#define N_PARITY   2   // 2 possible corner parities
	#define N_URFtoDLF 20160     // 8!/(8-6)! permutation of URF,UFL,ULB,UBR,DFR,DLF corners
	#define N_FRtoBR   11880     // 12!/(12-4)! permutation of FR,FL,BL,BR edges
  #define N_URtoUL   1320      // 12!/(12-3)! permutation of UR,UF,UL edges
	#define N_UBtoDF   1320      // 12!/(12-3)! permutation of UB,DR,DF edges
	#define N_URtoDF   20160     // 8!/(8-6)! permutation of UR,UF,UL,UB,DR,DF edges in phase2

	#define N_URFtoDLB  40320       // 8! permutations of the corners
	#define N_URtoBR    479001600  // 8! permutations of the corners

	#define N_MOVE    18


  struct _CoordCube
	{  // All coordinates are 0 for a solved cube except for UBtoDF, which is 114
  	 short twist;
  	 short flip;
  	 short parity;
  	 short FRtoBR;
  	 short URFtoDLF;
  	 short URtoUL;
  	 short UBtoDF;
	   int URtoDF;
  };
  typedef struct _CoordCube *CoordCube;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Generate a CoordCube from a CubieCube
	CoordCube CoordCube_construct(CoordCube this, CubieCube c) {
		this->twist = CubieCube_getTwist(c);
		this->flip = CubieCube_getFlip(c);
		this->parity = CubieCube_cornerParity(c);
		this->FRtoBR = CubieCube_getFRtoBR(c);
		this->URFtoDLF = CubieCube_getURFtoDLF(c);
		this->URtoUL = CubieCube_getURtoUL(c);
		this->UBtoDF = CubieCube_getUBtoDF(c);
		this->URtoDF = CubieCube_getURtoDF(c);// only needed in phase2
    return this;
	}

	// ******************************************Phase 1 move tables*****************************************************

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Move table for the twists of the corners
	// twist < 2187 in phase 2.
	// twist = 0 in phase 2.
	short twistMove[N_TWIST][N_MOVE];
  void CoordCube_initstatic_1() {
    struct _CubieCube _a;
		CubieCube a = CubieCube_construct_nopar(&_a);
    short i;
		for (i = 0; i < N_TWIST; i++) {
			CubieCube_setTwist(a,i);
      int j;
			for (j = 0; j < 6; j++) {
        int k;
				for (k = 0; k < 3; k++) {
					CubieCube_cornerMultiply(a,moveCube[j]);
					twistMove[i][3 * j + k] = CubieCube_getTwist(a);
				}
				CubieCube_cornerMultiply(a,moveCube[j]);// 4. faceturn restores
				// a
			}
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Move table for the flips of the edges
	// flip < 2048 in phase 1
	// flip = 0 in phase 2.
	short flipMove[N_FLIP][N_MOVE];
	void CoordCube_initstatic_2() {
    struct _CubieCube _a;
    CubieCube a = CubieCube_construct_nopar(&_a);
    short i;
		for (i = 0; i < N_FLIP; i++) {
			CubieCube_setFlip(a,i);
      int j;
			for (j = 0; j < 6; j++) {
        int k;
				for (k = 0; k < 3; k++) {
					CubieCube_edgeMultiply(a,moveCube[j]);
					flipMove[i][3 * j + k] = CubieCube_getFlip(a);
				}
				CubieCube_edgeMultiply(a,moveCube[j]);
				// a
			}
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Parity of the corner permutation. This is the same as the parity for the edge permutation of a valid cube.
	// parity has values 0 and 1
	short parityMove[2][18] = { { 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1 },
			{ 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 } };

	// ***********************************Phase 1 and 2 movetable********************************************************

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Move table for the four UD-slice edges FR, FL, Bl and BR
	// FRtoBRMove < 11880 in phase 1
	// FRtoBRMove < 24 in phase 2
	// FRtoBRMove = 0 for solved cube
	short FRtoBR_Move[N_FRtoBR][N_MOVE];
	void CoordCube_initstatic_3() {
    struct _CubieCube _a;
    CubieCube a = CubieCube_construct_nopar(&_a);
    short i;
		for (i = 0; i < N_FRtoBR; i++) {
			CubieCube_setFRtoBR(a,i);
      int j;
			for (j = 0; j < 6; j++) {
        int k;
				for (k = 0; k < 3; k++) {
					CubieCube_edgeMultiply(a,moveCube[j]);
					FRtoBR_Move[i][3 * j + k] = CubieCube_getFRtoBR(a);
				}
				CubieCube_edgeMultiply(a,moveCube[j]);
			}
		}
	}

	// *******************************************Phase 1 and 2 movetable************************************************

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Move table for permutation of six corners. The positions of the DBL and DRB corners are determined by the parity.
	// URFtoDLF < 20160 in phase 1
	// URFtoDLF < 20160 in phase 2
	// URFtoDLF = 0 for solved cube.
	short URFtoDLF_Move[N_URFtoDLF][N_MOVE];
	void CoordCube_initstatic_4() {
    struct _CubieCube _a;
    CubieCube a = CubieCube_construct_nopar(&_a);
    short i;
		for (i = 0; i < N_URFtoDLF; i++) {
			CubieCube_setURFtoDLF(a,i);
      int j,k;
			for (j = 0; j < 6; j++) {
				for (k = 0; k < 3; k++) {
					CubieCube_cornerMultiply(a,moveCube[j]);
					URFtoDLF_Move[i][3 * j + k] = CubieCube_getURFtoDLF(a);
				}
				CubieCube_cornerMultiply(a,moveCube[j]);
			}
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Move table for the permutation of six U-face and D-face edges in phase2. The positions of the DL and DB edges are
	// determined by the parity.
	// URtoDF < 665280 in phase 1
	// URtoDF < 20160 in phase 2
	// URtoDF = 0 for solved cube.
	short URtoDF_Move[N_URtoDF][N_MOVE];
	void CoordCube_initstatic_5() {
    struct _CubieCube _a;
    CubieCube a = CubieCube_construct_nopar(&_a);
    short i;
		for (i = 0; i < N_URtoDF; i++) {
			CubieCube_setURtoDF(a,i);
      int j,k;
			for (j = 0; j < 6; j++) {
				for (k = 0; k < 3; k++) {
					CubieCube_edgeMultiply(a,moveCube[j]);
					URtoDF_Move[i][3 * j + k] = (short) CubieCube_getURtoDF(a);
					// Table values are only valid for phase 2 moves!
					// For phase 1 moves, casting to short is not possible.
				}
				CubieCube_edgeMultiply(a,moveCube[j]);
			}
		}
	}

	// **************************helper move tables to compute URtoDF for the beginning of phase2************************

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Move table for the three edges UR,UF and UL in phase1.
	short URtoUL_Move[N_URtoUL][N_MOVE];
	void CoordCube_initstatic_6() {
    struct _CubieCube _a;
    CubieCube a = CubieCube_construct_nopar(&_a);
    short i;
		for (i = 0; i < N_URtoUL; i++) {
			CubieCube_setURtoUL(a,i);
      int j,k;
			for (j = 0; j < 6; j++) {
				for (k = 0; k < 3; k++) {
					CubieCube_edgeMultiply(a,moveCube[j]);
					URtoUL_Move[i][3 * j + k] = CubieCube_getURtoUL(a);
				}
				CubieCube_edgeMultiply(a,moveCube[j]);
			}
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Move table for the three edges UB,DR and DF in phase1.
	short UBtoDF_Move[N_UBtoDF][N_MOVE];
	void CoordCube_initstatic_7() {
    struct _CubieCube _a;
    CubieCube a = CubieCube_construct_nopar(&_a);
    short i;
		for (i = 0; i < N_UBtoDF; i++) {
			CubieCube_setUBtoDF(a,i);
      int j,k;
			for (j = 0; j < 6; j++) {
				for (k = 0; k < 3; k++) {
					CubieCube_edgeMultiply(a,moveCube[j]);
					UBtoDF_Move[i][3 * j + k] = CubieCube_getUBtoDF(a);
				}
				CubieCube_edgeMultiply(a,moveCube[j]);
			}
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Table to merge the coordinates of the UR,UF,UL and UB,DR,DF edges at the beginning of phase2
	short MergeURtoULandUBtoDF[336][336];
	void CoordCube_initstatic_8() {
		// for i, j <336 the six edges UR,UF,UL,UB,DR,DF are not in the
		// UD-slice and the index is <20160
    short uRtoUL, uBtoDF;
		for (uRtoUL = 0; uRtoUL < 336; uRtoUL++) {
			for (uBtoDF = 0; uBtoDF < 336; uBtoDF++) {
				MergeURtoULandUBtoDF[uRtoUL][uBtoDF] = (short) getURtoDF(uRtoUL, uBtoDF);
			}
		}
	}

	// ****************************************Pruning tables for the search*********************************************

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Set pruning value in table. Two values are stored in one char.
   void setPruning(char *table, int index, char value) {
		if ((index & 1) == 0)
			table[index / 2] &= 0xf0 | value;
		else
			table[index / 2] &= 0x0f | (value << 4);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Extract pruning value
   char getPruning(char *table, int index) {
		if ((index & 1) == 0)
			return (char) (table[index / 2] & 0x0f);
		else
			return (char) ((table[index / 2]>>4) & 0x0f);
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Pruning table for the permutation of the corners and the UD-slice edges in phase2.
	// The pruning table entries give a lower estimation for the number of moves to reach the solved cube.
	char Slice_URFtoDLF_Parity_Prun [N_SLICE2 * N_URFtoDLF * N_PARITY / 2];
	void CoordCube_initstatic_9() {
    int i;
		for (i = 0; i < N_SLICE2 * N_URFtoDLF * N_PARITY / 2; i++)
			Slice_URFtoDLF_Parity_Prun[i] = -1;
		int depth = 0;
		setPruning(Slice_URFtoDLF_Parity_Prun, 0, (char) 0);
		int done = 1;
		while (done != N_SLICE2 * N_URFtoDLF * N_PARITY) {
			for (i = 0; i < N_SLICE2 * N_URFtoDLF * N_PARITY; i++) {
				int parity = i % 2;
				int URFtoDLF = (i / 2) / N_SLICE2;
				int slice = (i / 2) % N_SLICE2;
				if (getPruning(Slice_URFtoDLF_Parity_Prun, i) == depth) {
          int j;
					for (j = 0; j < 18; j++) {
						switch (j) {
						case 3:
						case 5:
						case 6:
						case 8:
						case 12:
						case 14:
						case 15:
						case 17:
							continue;
						default: 1;
							int newSlice = FRtoBR_Move[slice][j];
							int newURFtoDLF = URFtoDLF_Move[URFtoDLF][j];
							int newParity = parityMove[parity][j];
							if (getPruning(Slice_URFtoDLF_Parity_Prun, (N_SLICE2 * newURFtoDLF + newSlice) * 2 + newParity) == 0x0f) {
								setPruning(Slice_URFtoDLF_Parity_Prun, (N_SLICE2 * newURFtoDLF + newSlice) * 2 + newParity,
										(char) (depth + 1));
								done++;
							}
						}
					}
				}
			}
			depth++;
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Pruning table for the permutation of the edges in phase2.
	// The pruning table entries give a lower estimation for the number of moves to reach the solved cube.
	char Slice_URtoDF_Parity_Prun[N_SLICE2 * N_URtoDF * N_PARITY / 2];
	void CoordCube_initstatic_10() {
    int i;
		for (i = 0; i < N_SLICE2 * N_URtoDF * N_PARITY / 2; i++)
			Slice_URtoDF_Parity_Prun[i] = -1;
		int depth = 0;
		setPruning(Slice_URtoDF_Parity_Prun, 0, (char) 0);
		int done = 1;
		while (done != N_SLICE2 * N_URtoDF * N_PARITY) {
			for (i = 0; i < N_SLICE2 * N_URtoDF * N_PARITY; i++) {
				int parity = i % 2;
				int URtoDF = (i / 2) / N_SLICE2;
				int slice = (i / 2) % N_SLICE2;
				if (getPruning(Slice_URtoDF_Parity_Prun, i) == depth) {
          int j;
					for (j = 0; j < 18; j++) {
						switch (j) {
						case 3:
						case 5:
						case 6:
						case 8:
						case 12:
						case 14:
						case 15:
						case 17:
							continue;
						default: 1;
							int newSlice = FRtoBR_Move[slice][j];
							int newURtoDF = URtoDF_Move[URtoDF][j];
							int newParity = parityMove[parity][j];
							if (getPruning(Slice_URtoDF_Parity_Prun, (N_SLICE2 * newURtoDF + newSlice) * 2 + newParity) == 0x0f) {
								setPruning(Slice_URtoDF_Parity_Prun, (N_SLICE2 * newURtoDF + newSlice) * 2 + newParity,
										(char) (depth + 1));
								done++;
							}
						}
					}
				}
			}
			depth++;
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Pruning table for the twist of the corners and the position (not permutation) of the UD-slice edges in phase1
	// The pruning table entries give a lower estimation for the number of moves to reach the H-subgroup.
	char Slice_Twist_Prun[N_SLICE1 * N_TWIST / 2 + 1];
	void CoordCube_initstatic_11() {
    int i;
		for (i = 0; i < N_SLICE1 * N_TWIST / 2 + 1; i++)
			Slice_Twist_Prun[i] = -1;
		int depth = 0;
		setPruning(Slice_Twist_Prun, 0, (char) 0);
		int done = 1;
		while (done != N_SLICE1 * N_TWIST) {
			for (i = 0; i < N_SLICE1 * N_TWIST; i++) {
				int twist = i / N_SLICE1, slice = i % N_SLICE1;
				if (getPruning(Slice_Twist_Prun, i) == depth) {
          int j;
					for (j = 0; j < 18; j++) {
						int newSlice = FRtoBR_Move[slice * 24][j] / 24;
						int newTwist = twistMove[twist][j];
						if (getPruning(Slice_Twist_Prun, N_SLICE1 * newTwist + newSlice) == 0x0f) {
							setPruning(Slice_Twist_Prun, N_SLICE1 * newTwist + newSlice, (char) (depth + 1));
							done++;
						}
					}
				}
			}
			depth++;
		}
	}

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Pruning table for the flip of the edges and the position (not permutation) of the UD-slice edges in phase1
	// The pruning table entries give a lower estimation for the number of moves to reach the H-subgroup.
	char Slice_Flip_Prun[N_SLICE1 * N_FLIP / 2];
	void CoordCube_initstatic_12() {
    int i;
		for (i = 0; i < N_SLICE1 * N_FLIP / 2; i++)
			Slice_Flip_Prun[i] = -1;
		int depth = 0;
		setPruning(Slice_Flip_Prun, 0, (char) 0);
		int done = 1;
		while (done != N_SLICE1 * N_FLIP) {
			for (i = 0; i < N_SLICE1 * N_FLIP; i++) {
				int flip = i / N_SLICE1, slice = i % N_SLICE1;
				if (getPruning(Slice_Flip_Prun, i) == depth) {
          int j;
					for (j = 0; j < 18; j++) {
						int newSlice = FRtoBR_Move[slice * 24][j] / 24;
						int newFlip = flipMove[flip][j];
						if (getPruning(Slice_Flip_Prun, N_SLICE1 * newFlip + newSlice) == 0x0f) {
							setPruning(Slice_Flip_Prun, N_SLICE1 * newFlip + newSlice, (char) (depth + 1));
							done++;
						}
					}
				}
			}
			depth++;
		}
	}


	// A move on the coordinate level
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	void CoordCube_move(CoordCube this, int m) {
		this->twist = twistMove[this->twist][m];
		this->flip = flipMove[this->flip][m];
		this->parity = parityMove[this->parity][m];
		this->FRtoBR = FRtoBR_Move[this->FRtoBR][m];
		this->URFtoDLF = URFtoDLF_Move[this->URFtoDLF][m];
		this->URtoUL = URtoUL_Move[this->URtoUL][m];
		this->UBtoDF = UBtoDF_Move[this->UBtoDF][m];
		if (this->URtoUL < 336 && this->UBtoDF < 336)// updated only if UR,UF,UL,UB,DR,DF
			// are not in UD-slice
			this->URtoDF = MergeURtoULandUBtoDF[this->URtoUL][this->UBtoDF];
	}


// --- Search.java


	int ax[31]; // The axis of the move
	int po[31]; // The power of the move

	int flip[31]; // phase1 coordinates
	int twist[31];
	int slice[31];

	int parity[31]; // phase2 coordinates
	int URFtoDLF[31];
	int FRtoBR[31];
	int URtoUL[31];
	int UBtoDF[31];
	int URtoDF[31];

	int minDistPhase1[31]; // IDA* distance do goal estimations
	int minDistPhase2[31];

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// generate the solution string from the array data
	void solutionToString(int length, char* buffer) {
		int bfill = 0;
		int i;
		for (i = 0; i < length; i++) {
			switch (ax[i]) {
			case 0:
				buffer[bfill++] = 'U';
				break;
			case 1:
				buffer[bfill++] = 'R';
				break;
			case 2:
				buffer[bfill++] = 'F';
				break;
			case 3:
				buffer[bfill++] = 'D';
				break;
			case 4:
				buffer[bfill++] = 'L';
				break;
			case 5:
				buffer[bfill++] = 'B';
				break;
			}
			switch (po[i]) {
			case 2:
				buffer[bfill++] = '2';
				break;
			case 3:
				buffer[bfill++] = '\'';
				break;
			}
		}
    buffer[bfill] = 0;
	};


	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Apply phase2 of algorithm and return the combined phase1 and phase2 depth. In phase2, only the moves
	// U,D,R2,F2,L2 and B2 are allowed.
 int totalDepth(int depthPhase1, int maxDepth, int maxDepthPhase2) {
		int mv = 0, d1 = 0, d2 = 0;
//		int maxDepthPhase2 = Math_min(10, maxDepth - depthPhase1);// Allow only max 10 moves in phase2
        int i;
		for (i = 0; i < depthPhase1; i++) {
			mv = 3 * ax[i] + po[i] - 1;
			URFtoDLF[i + 1] = URFtoDLF_Move[URFtoDLF[i]][mv];
			FRtoBR[i + 1] = FRtoBR_Move[FRtoBR[i]][mv];
			parity[i + 1] = parityMove[parity[i]][mv];
		}

		if ((d1 = getPruning(Slice_URFtoDLF_Parity_Prun,
				(N_SLICE2 * URFtoDLF[depthPhase1] + FRtoBR[depthPhase1]) * 2 + parity[depthPhase1])) > maxDepthPhase2)
			return -1;

		for (i = 0; i < depthPhase1; i++) {
			mv = 3 * ax[i] + po[i] - 1;
			URtoUL[i + 1] = URtoUL_Move[URtoUL[i]][mv];
			UBtoDF[i + 1] = UBtoDF_Move[UBtoDF[i]][mv];
		}
		URtoDF[depthPhase1] = MergeURtoULandUBtoDF[URtoUL[depthPhase1]][UBtoDF[depthPhase1]];

		if ((d2 = getPruning(Slice_URtoDF_Parity_Prun,
				(N_SLICE2 * URtoDF[depthPhase1] + FRtoBR[depthPhase1]) * 2 + parity[depthPhase1])) > maxDepthPhase2)
			return -1;

		if ((minDistPhase2[depthPhase1] = Math_max(d1, d2)) == 0)// already solved
			return depthPhase1;

		// now set up search

		int depthPhase2 = 1;
		int n = depthPhase1;
		int busy = 0;
		po[depthPhase1] = 0;
		ax[depthPhase1] = 0;
		minDistPhase2[n + 1] = 1;// else failure for depthPhase2=1, n=0
		// +++++++++++++++++++ end initialization +++++++++++++++++++++++++++++++++
		do {
			do {
				if ((depthPhase1 + depthPhase2 - n > minDistPhase2[n + 1]) && !busy) {

					if (ax[n] == 0 || ax[n] == 3)// Initialize next move
					{
						ax[++n] = 1;
						po[n] = 2;
					} else {
						ax[++n] = 0;
						po[n] = 1;
					}
				} else if ((ax[n] == 0 || ax[n] == 3) ? (++po[n] > 3) : ((po[n] = po[n] + 2) > 3)) {
					do {// increment axis
						if (++ax[n] > 5) {
							if (n == depthPhase1) {
								if (depthPhase2 >= maxDepthPhase2)
									return -1;
								else {
									depthPhase2++;
									ax[n] = 0;
									po[n] = 1;
									busy = 0;
									break;
								}
							} else {
								n--;
								busy = 1;
								break;
							}

						} else {
							if (ax[n] == 0 || ax[n] == 3)
								po[n] = 1;
							else
								po[n] = 2;
							busy = 0;
						}
					} while (n != depthPhase1 && (ax[n - 1] == ax[n] || ax[n - 1] - 3 == ax[n]));
				} else
					busy = 0;
			} while (busy);
			// +++++++++++++ compute new coordinates and new minDist ++++++++++
			mv = 3 * ax[n] + po[n] - 1;

			URFtoDLF[n + 1] = URFtoDLF_Move[URFtoDLF[n]][mv];
			FRtoBR[n + 1] = FRtoBR_Move[FRtoBR[n]][mv];
			parity[n + 1] = parityMove[parity[n]][mv];
			URtoDF[n + 1] = URtoDF_Move[URtoDF[n]][mv];

			minDistPhase2[n + 1] = Math_max(getPruning(Slice_URtoDF_Parity_Prun, (N_SLICE2
					* URtoDF[n + 1] + FRtoBR[n + 1])
					* 2 + parity[n + 1]), getPruning(Slice_URFtoDLF_Parity_Prun, (N_SLICE2
					* URFtoDLF[n + 1] + FRtoBR[n + 1])
					* 2 + parity[n + 1]));
			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		} while (minDistPhase2[n + 1] != 0);
		return depthPhase1 + depthPhase2;
	}

	/**
	 * Computes the solver string for a given cube.
	 *
	 * @param facelets
	 *          is the cube definition string, see {@link Facelet} for the format.
	 *
	 * @param maxDepth
	 *          defines the maximal allowed maneuver length. For random cubes, a maxDepth of 21 usually will return a
	 *          solution in less than 0.5 seconds. With a maxDepth of 20 it takes a few seconds on average to find a
	 *          solution, but it may take much longer for specific cubes.
	 *
	 * @return The solution string or an error code:<br>
	 *         Error 1: There is not exactly one facelet of each colour<br>
	 *         Error 2: Not all 12 edges exist exactly once<br>
	 *         Error 3: Flip error: One edge has to be flipped<br>
	 *         Error 4: Not all corners exist exactly once<br>
	 *         Error 5: Twist error: One corner has to be twisted<br>
	 *         Error 6: Parity error: Two corners or two edges have to be exchanged<br>
	 *         Error 7: No solution exists for the given maxDepth<br>
	 *         Error 8: Timeout, no solution within given time
	 */
	void solution(char* facelets, int maxDepth, int maxDepthPhase2, char* outbuffer, void (*phase1finishcallback)()) {
		int s;

		// +++++++++++++++++++++check for wrong input +++++++++++++++++++++++++++++
    struct _FaceCube _fc;
		FaceCube fc = FaceCube_construct(&_fc, facelets);
    if ((s=FaceCube_verifyCounts(fc)!=0))
    {  sprintf(outbuffer, "Error %d", s);
        return;
    }
		struct _CubieCube _cc;
		CubieCube cc = CubieCube_constructFromFaceCube(&_cc,fc);
		if ((s=CubieCube_verify(cc)) != 0)
		{	sprintf(outbuffer, "Error %d", s);
			return;
		}

		// +++++++++++++++++++++++ initialization +++++++++++++++++++++++++++++++++
		struct _CoordCube _c;
		CoordCube c = CoordCube_construct(&_c,cc);

		po[0] = 0;
		ax[0] = 0;
		flip[0] = c->flip;
		twist[0] = c->twist;
		parity[0] = c->parity;
		slice[0] = c->FRtoBR / 24;
		URFtoDLF[0] = c->URFtoDLF;
		FRtoBR[0] = c->FRtoBR;
		URtoUL[0] = c->URtoUL;
		UBtoDF[0] = c->UBtoDF;

		minDistPhase1[1] = 1;// else failure for depth=1, n=0
		int mv = 0, n = 0;
		int busy = 0;
		int depthPhase1 = 1;

//		long tStart = System.currentTimeMillis();

		// +++++++++++++++++++ Main loop ++++++++++++++++++++++++++++++++++++++++++
		do {
			do {
				if ((depthPhase1 - n > minDistPhase1[n + 1]) && !busy) {

					if (ax[n] == 0 || ax[n] == 3)// Initialize next move
						ax[++n] = 1;
					else
						ax[++n] = 0;
					po[n] = 1;
				} else if (++po[n] > 3) {
					do {// increment axis
						if (++ax[n] > 5) {

//							if (System.currentTimeMillis() - tStart > timeOut << 10)
//								return "Error 8";

							if (n == 0) {
								if (depthPhase1 >= maxDepth)
								{ sprintf (outbuffer, "Error 7");
									return;
								}
								else {
									depthPhase1++;
									ax[n] = 0;
									po[n] = 1;
									busy = 0;
									break;
								}
							} else {
								n--;
								busy = 1;
								break;
							}

						} else {
							po[n] = 1;
							busy = 0;
						}
					} while (n != 0 && (ax[n - 1] == ax[n] || ax[n - 1] - 3 == ax[n]));
				} else
					busy = 0;
			} while (busy);

			// +++++++++++++ compute new coordinates and new minDistPhase1 ++++++++++
			// if minDistPhase1 =0, the H subgroup is reached
			mv = 3 * ax[n] + po[n] - 1;
			flip[n + 1] = flipMove[flip[n]][mv];
			twist[n + 1] = twistMove[twist[n]][mv];
			slice[n + 1] = FRtoBR_Move[slice[n] * 24][mv] / 24;
			minDistPhase1[n + 1] = Math_max(getPruning(Slice_Flip_Prun, N_SLICE1 * flip[n + 1]
					+ slice[n + 1]), getPruning(Slice_Twist_Prun, N_SLICE1 * twist[n + 1]
					+ slice[n + 1]));
			// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

			if (minDistPhase1[n + 1] == 0 && n >= depthPhase1 - 5) {
				minDistPhase1[n + 1] = 10;// instead of 10 any value >5 is possible
				if (n == depthPhase1 - 1) {
				  if (phase1finishcallback!=0)
				  {	 solutionToString(depthPhase1, outbuffer);
					   (*phase1finishcallback)();
				  }
				  s = totalDepth(depthPhase1, maxDepth, Math_min(maxDepth-depthPhase1,maxDepthPhase2));
				  if (s >= 0) {
  					if (s == depthPhase1
	 						|| (ax[depthPhase1 - 1] != ax[depthPhase1] && ax[depthPhase1 - 1] != ax[depthPhase1] + 3)
              || (phase1finishcallback!=0)  // when already having sent phase1, do not change your mind about it
            )
					 {	solutionToString(s, outbuffer);
					   	return;
					 }
				  }
				}

			}
		} while (1);
	}


// ---- static initializers of twophase package

int get_twophase_cacheable_areas(void** ptr, int* len)
{
	ptr[0] = &Slice_URFtoDLF_Parity_Prun;
	len[0] = sizeof(Slice_URFtoDLF_Parity_Prun);
	ptr[1] = &Slice_URtoDF_Parity_Prun;
	len[1] = sizeof(Slice_URtoDF_Parity_Prun);
	ptr[2] = &Slice_Twist_Prun;
	len[2] = sizeof(Slice_Twist_Prun);
	ptr[3] = &Slice_Flip_Prun;
	len[3] = sizeof(Slice_Flip_Prun);
	return 4;
}

void twophase_init()
{
  CubieCube_initstatic();
  
  CoordCube_initstatic_1();
  CoordCube_initstatic_2();
  CoordCube_initstatic_3();
  CoordCube_initstatic_4();
  CoordCube_initstatic_5();
  CoordCube_initstatic_6();
  CoordCube_initstatic_7();
  CoordCube_initstatic_8();
  CoordCube_initstatic_9();
	CoordCube_initstatic_10();
	CoordCube_initstatic_11();
	CoordCube_initstatic_12();
}

