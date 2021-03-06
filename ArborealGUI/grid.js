//Danny Radosevich
//Code to handle the grid layout for the Elements
/*---------------------------------Code for Grid Elements---------------------------------*/


//var grid = document.querySelector('.grid');
//var msnry = new Masonry(grid, {columnWidth:160, itemSelector:'.grid-item'});
var itemID = 0; //helps keep track of elements
var elems = new Array();

/*-------------------------------Code for Addding Elements-------------------------------*/
/*---------------------------------------------------------------------------------------*/
// now the function to add elements dynamically

function addToGrid(elemType, caption)
{
  //alert("adding");
  //create the ID for the element to be accessed
  if(caption.includes('['))
  {
    caption = caption.replace('[','');
  }
  if(caption.includes(']'))
  {
    caption = caption.replace(']','');
  }
  if(caption.match(/[a-z]*/))
  {
      var id = elemType + itemID;
      itemID++;
      //add id to an array to keep track
      elems.push(id);

      //set up the figure for dispaly
      var newFig = document.createElement("img");
      var divFig = document.createElement("div");
      var text = document.createTextNode(caption);
      var br = document.createElement("br");


      if(elemType=='tag')
      {
        newFig.src = './res/folder.png';
      }
      else if (elemType=='file')
      {
        newFig.src = './res/icon.png';
      }

      //Setting up the figure
      //newFig.src = 'https://cdn.bulbagarden.net/upload/7/73/004Charmander.png';
      //newFig.src = './res/icon.png';
      newFig.height = '70';
      newFig.width = '70';
      //newFig.id = id;
      //newFig.setAttribute("onclick", "testOn("+id+")");
      newFig.addEventListener("dblclick", itemClick.bind(null,id,caption));

      // creating the text below the figure

      //adding the picture to the display area
      divFig.appendChild(newFig);
      divFig.appendChild(br);
      divFig.appendChild(text);
      divFig.setAttribute("class","image");
      divFig.setAttribute("onmousedown", "iconRightClick()")
      divFig.style.padding = "5px 5px 5px 5px";
      divFig.id=id;
      //text.setAttribute("align","bottom");
      document.getElementById('grid').appendChild(divFig);
}
}
function clearGrid() //empties the grid and the elements in the array
{
  for(var i=0; i <elems.length; i++)
  {
    var toRemove = document.getElementById(elems[i]);
    toRemove.parentNode.removeChild(toRemove);
  }
  elems = [];
}
function itemClick(id,caption) // checks if an item is clicked.
//Performs a function based on if it is a file, or a tag
{
  //alert("image clicked: "+id);
  if(id.includes("tag")) // for when a tag is double clicked, will find all files assoiciated with tag
  {
    clearGrid();
    getFilesforTag(caption);
  }
  else if(id.includes("file")) // for when a file is double clicked
  {
    alert("You clicked on: " + caption+ "\n open coming soon");
    //does nothing
  }
}
