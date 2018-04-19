//Danny Radosevich
// Code for all the on click listeners
/* Code to deal with button presses for the GUI*/
/*--------------------------------Code for Nav Bar Buttons--------------------------------*/
function arborealButton()
{
  //Generate a pop up about Arboreal
  window.alert("Arboreal is a new kind of file system\nthat uses a tree based structure!")
}
function helpButton()
{
  // open a  help menu for the functionality of the program
}
function homeButton()
{
  //go to the "root" directory
  // will store the top tags somewhere, and re set up with top tags
}
function searchButton()
{
  //get the text written in the search bar and search the file system for this
  //var myDoc = "./index.html";
  var inputtedValue = document.getElementById("search_bar").value;
  var inArray = inputtedValue.split(", ");
  window.alert(inArray[0]);
  //window.alert(inputtedValue);
  //var test = "heyyyyy";
  //window.alert("Heyyyyyy");
}
function searchHelp()
{
  var lineOne= "Howdy, here is the functionality for the searchbar:\n";
  var lineTwo= "You can search for a file name, or a tag.\n"
  var lineThree="To enter multiple tags follow this format:\n"
  var lineFour="OR operator: tag_one, tag_two\n"
  var lineFive="AND operator: tag_one& tag_two\n"
  var lineSix ="You can even intermix as such:\n"
  var lineSeven="tag_one, tag_two & tag_three, tag_four\n"
  var lineEight="This will get you tag_one, tag_four, or tag_two and tag_three."
  window.alert(lineOne+lineTwo+lineThree+lineFour+lineFive+lineSix+lineSeven+lineEight);
}
function backButton()
{
  //goes back up in the directory
  window.alert("The Game\n(you lost)");

}

/*--------------------------------Code for Nav Bar Buttons--------------------------------*/
/*----------------------------------------------------------------------------------------*/
/*-----------------------------------Window interaction-----------------------------------*/

function bodyRightClick() //for a right click on the body area,
{
  var rightclick;
  var eve = window.event;
  if ((eve.which && eve.which == 3) || (eve.button && eve.button == 2))
  {
    alert("right click");
  }
}
function iconRightClick() // check for a right click on an icon
{
  var rightclick;
  var eve = window.event;
  if ((eve.which && eve.which == 3) || (eve.button && eve.button == 2))
  {
    alert("image right clicked");
  }
}
