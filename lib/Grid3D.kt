/* see notes in Grid.kt */
package draftyhat



/* grid is read from top left to bottom right */
class Point3D(
    val x: Int,
    val y: Int,
    val z: Int) {
  constructor(s: String) : this(s.trim('(').split(',')[0].toInt(),
      s.trim(')').split(',')[1].toInt(),
      s.trim(')').split(',')[2].toInt())

  operator fun plus(p: Point3D): Point3D {
    return Point3D(x + p.x, y + p.y, z + p.z)
  }
  operator fun minus(p: Point3D): Point3D {
    return Point3D(x - p.x, y - p.y, z - p.z)
  }
  override operator fun equals(other: Any?): Boolean {
    return other is Point3D && other.x == x && other.y == y && other.z == z
  }
  override fun toString(): String {
    return "($x,$y,$z)"
  }
  override fun hashCode(): Int {
    return ((x.hashCode() * y).hashCode() * z).hashCode()
  }
}

class Face(val a: Point3D, val b: Point3D) {
  override operator fun equals(other: Any?): Boolean {
    return other is Face && other.a == a && other.b == b
  }
  override fun hashCode(): Int {
    return (a.hashCode() + (b.x.hashCode() * b.y + b.z)).hashCode()
  }
  override fun toString(): String {
    return "Face($a,$b)"
  }
}

/* a box is delineated by the two opposite corners */
/* assumes a > b for every dimension */
class Box(val a: Point3D, val b: Point3D) {
  init {
    if(b.x < a.x || b.y < a.y || b.z < a.z)
      throw Exception("ERROR: starting point must be strictly greater" +
          " than opposite corner. Try" +
         " ${Point3D(Math.min(a.x,b.x), Math.min(a.y,b.y), Math.min(a.z,b.z))}," +
         " ${Point3D(Math.max(a.x,b.x), Math.max(a.y,b.y), Math.max(a.z,b.z))}.")
  }
  override fun hashCode(): Int {
    return (a.hashCode() +
      (b.x-a.x)*(b.x-a.x) + ((b.y-a.y)*(b.y-a.y)) + ((b.z-a.z)*(b.z-a.z))).hashCode()
  }
  override operator fun equals(other: Any?): Boolean {
    return other is Box && other.a == a && other.b == b
  }

  fun getFaces(): MutableList<Face> {
    /* get a list of the faces of this box */
    val retval = mutableListOf<Face>()
    for(xIter in a.x until b.x) {
      for(yIter in a.y until b.y) {
        /* bottom faces */
        retval.add(Face(Point3D(xIter,yIter,a.z),
              Point3D(xIter + 1, yIter + 1, a.z)))
        /* top faces */
        retval.add(Face(Point3D(xIter,yIter,b.z),
              Point3D(xIter + 1, yIter + 1, b.z)))
      }
      for(zIter in a.z until b.z) {
        /* front faces */
        retval.add(Face(Point3D(xIter,a.y,zIter),
              Point3D(xIter + 1, a.y, zIter + 1)))
        /* back faces */
        retval.add(Face(Point3D(xIter,b.y,zIter),
              Point3D(xIter + 1, b.y, zIter + 1)))
      }
    }
    for(yIter in a.y until b.y) {
      for(zIter in a.z until b.z) {
        /* left faces */
        retval.add(Face(Point3D(a.x,yIter,zIter),
              Point3D(a.x, yIter + 1, zIter + 1)))
        /* right faces */
        retval.add(Face(Point3D(b.x,yIter,zIter),
              Point3D(b.x, yIter + 1, zIter + 1)))
      }
    }

    return retval
  }

  /* get this box's 6 closest neighbors (no diagonals). The unknown
     dimension of each neighbor will be 1. */
  fun getNeighbors(): List<Box> {
    return listOf(
        /* front */
        Box(Point3D(a.x, a.y - 1, a.z),
            Point3D(b.x, a.y,     b.z)),
        /* top */
        Box(Point3D(a.x, a.y, b.z    ),
            Point3D(b.x, b.y, b.z + 1)),
        /* right */
        Box(Point3D(b.x    , a.y, a.z),
            Point3D(b.x + 1, b.y, b.z)),
        /* left */
        Box(Point3D(a.x - 1, a.y, a.z),
            Point3D(a.x    , b.y, b.z)),
        /* back */
        Box(Point3D(a.x, b.y    , a.z),
            Point3D(b.x, b.y + 1, b.z)),
        /* bottom */
        Box(Point3D(a.x, a.y, a.z - 1),
            Point3D(b.x, b.y, a.z    )),
    )
  }

  override fun toString(): String {
    return "Box($a,$b)"
  }
}





/* *** test code *** */

data class Point3DReadTestcase(
  val inputLine: String,
  val point: Point3D,
)
data class Box3DFaceTestcase(
  val inputLines: Array<String>,
  val expectedAnswerSize: Int
)


/*
fun main() {
  // run unit tests
  var ranN = 0
  var succeededN = 0

  // get() test
  val Point3DReadTestcase = arrayOf(
        Point3DReadTestcase("0,1,2", Point3D(0,1,2)),
        Point3DReadTestcase("3,2,1", Point3D(3,2,1)),
        Point3DReadTestcase("-6,-3,-2", Point3D(-6,-3,-2)),
        Point3DReadTestcase("12345,65432,-23456", Point3D(12345,65432,-23456)),
      )
  for(testcase in Point3DReadTestcase) {
    ranN += 1
    try {
      val answer = Point3D(testcase.inputLine)
      if(answer != testcase.point)
        throw Exception("${testcase.inputLine} -> ${testcase.point}  got $answer instead")
      succeededN += 1
    } catch(e: Exception) {
      println("FAILED: ${e.message}")
    }
  }

  val successstring = if(succeededN == ranN) "SUCCESS" else "FAIL"
  println("$successstring  passed $succeededN/$ranN tests")
}
*/
