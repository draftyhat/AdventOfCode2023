/* import org.junit.Assert.assertEquals   /* for unit testing */
   F THIS
   cannot for the life of me find org.junit.Assert in classpath on command line
   java -cp <junit.jar> -jar Grid.jar
   Exception in thread "main" java.lang.NoClassDefFoundError: org/junit/Assert
*/
/* compile with
   kotlinc Grid.kt -include-runtime -d Grid.jar */

/* I tried to make this generic, but that is very convoluted in Kotlin
   https://stackoverflow.com/questions/35581867/kotlin-ndarray-with-a-lambda-constructor-with-generic-return-type
   so it's a grid of integers. If I need characters later, maybe I can store the characters as ints.
*/
package draftyhat



/* grid is read from top left to bottom right */
class Point(
    val x: Int,
    val y: Int) {
  constructor(s: String) : this(s.trim('(').split(',')[0].toInt(),
      s.trim(')').split(',')[1].toInt())

  operator fun plus(p: Point): Point {
    return Point(x + p.x, y + p.y)
  }
  operator fun minus(p: Point): Point {
    return Point(x - p.x, y - p.y)
  }
  override operator fun equals(other: Any?): Boolean {
    return other is Point && other.x == x && other.y == y
  }
  override fun toString(): String {
    return "($x,$y)"
  }
  override fun hashCode(): Int {
    return (x.hashCode() * y).hashCode()
  }
}

class Grid (
    var width: Int,
    var height: Int,
    init: (column: Int, row: Int, opaque:Any) -> Int,
    opaqueInitArgument: Any) {
  private var gridData = Array<Int>(width * height,
      {index -> init(index % width, index / width, opaqueInitArgument)})

  operator fun get(x: Int, y: Int) : Int {
    return gridData[y * width + x]
  }
  operator fun get(p: Point) : Int { return get(p.x, p.y) }
  operator fun set(x: Int, y: Int, value: Int) : Int {
    val tmp = gridData[y * width + x]
    gridData[y * width + x] = value
    return tmp
  }
  operator fun set(p: Point, z: Int) : Int { return set(p.x, p.y, z) }

  fun neighbors(x: Int, y: Int, diagonals: Boolean = false):MutableList<Point> {
    val retval = mutableListOf<Point>()
    if((x - 1) >= 0) {
      retval.add(Point(x - 1, y))
      if(diagonals) {
        if((y - 1) >= 0)
          retval.add(Point(x - 1, y - 1))
        if((y + 1) < height)
          retval.add(Point(x - 1, y + 1))
      }
    }
    if((y - 1) >= 0) {
      retval.add(Point(x, y - 1))
    }
    if((x + 1) < width) {
      retval.add(Point(x + 1, y))
      if(diagonals) {
        if((y - 1) >= 0)
          retval.add(Point(x + 1, y - 1))
        if((y + 1) < height)
          retval.add(Point(x + 1, y + 1))
      }
    }
    if((y + 1) < height) {
      retval.add(Point(x, y + 1))
    }
    return retval
  }
  fun neighbors(p: Point, diagonals: Boolean = false): MutableList<Point> {
    return neighbors(p.x, p.y, diagonals)
  }
  fun sum(): Int {
    /* sum all elements in the grid */
    return gridData.sum()
  }
  override fun toString(): String {
    /* this is awful */
    var retval = ""
    for(index in 0 until gridData.size) {
      if((index % width) == 0 && index != 0)
        retval = retval + "\n"
      retval = retval + gridData[index].toString()
    }
    return retval
  }
}


fun readGridItem(column: Int, row: Int, opaque:Any): Int {
  @Suppress("UNCHECKED_CAST")
  val inputLines = opaque as List<String>
  return inputLines[row][column].toString().toInt()
}


/* *** test code *** */

data class GridGetTestcase(
  val inputLines: Array<String>,
  val point: Point,
  val expectedAnswer: Int
)

data class GridNeighborTestcase(
  val inputLines: Array<String>,
  val point: Point,
  val expectedAnswer: Array<Point>
)


/*
fun main() {
  // run unit tests
  var ranN = 0
  var succeededN = 0

  // get() test
  val gridGetTestcases = arrayOf(
        GridGetTestcase(listOf("012","345","678","901"),
          Point(0,0), 0),
        GridGetTestcase(listOf("012","345","678","901"),
          Point(1,0), 1),
        GridGetTestcase(listOf("012","345","678","901"),
          Point(2,0), 2),
      )
  for(testcase in gridGetTestcases) {
    val grid = Grid(testcase.inputLines[0].length, testcase.inputLines.size,
        { column, row, opaque -> readGridItem(column, row, opaque)},
        testcase.inputLines)
    val answer = grid[testcase.point]
    ranN += 1
    try {
      //assertEquals(
      //    "${testcase.point} -> ${testcase.expectedAnswer} got $answer instead",
      //    testcase.expectedAnswer, answer)
      println("Grid:")
      println(grid)
      if(testcase.expectedAnswer != answer)
        throw Exception("${testcase.point} -> ${testcase.expectedAnswer} got $answer instead")
      succeededN += 1
    } catch(e: Exception) {
      println("FAILED: ${e.message}")
    }
      
  }

  val successstring = if(succeededN == ranN) "SUCCESS" else "FAIL"
  println("$successstring  passed $succeededN/$ranN tests")
}
*/
