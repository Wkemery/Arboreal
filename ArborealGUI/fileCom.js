//Danny Radoseivch
//Code to interact with the Arboreal file system
//328 -372
//send single dolar sign signal

/*------------------------------Static Interaction Variables------------------------------*/
//codes for renaming a file
const RNAME_FP= 100; //renames a file at specified path
const RNAME_TS= 101;//renames one or more tag
const RNAME_FS= 102;//renames files in current directory
//codes for opening a file
const OPEN_FP=  200; //  opens a file with file path
const OPEN_F =  201; //opens a file in current directory
//codes for new files/tags/path
const NEW_FP =  300; // create a new file path from anywhere
const NEW_TS =  301; // one or more new tags
const NEW_FS =  302; //1 or more new files within currrent directory
//codes for finding tags/files
const FIND_TS = 400; // find files by tags
const FIND_FS = 401; //find files by name
//codes for delelting files/tags
const DEL_FP =  500; //delete a file (with file path)
const DEL_TS =  501; // delete tag (must be emmpty)
const DEL_FS =  502; // delete file in current directory
//codes for closing a file
const CLOSE_FP= 600; // closes a file at specified path
const CLOSE_F = 600; // closes a file in current directory
//codes to get sttributes fo files
const ATTR_FP = 700; // get attributes of file at specified path
const ATTR_FS = 701; // Get file attributes in working directory
//codes for merging of tags
const MERG_1_1= 801; // merges on tag into another
const MERG_M_1= 802; // merges multiple tags into one
//codes to tag a files
const TAG_FP =  900; //tags file at specified path
const TAG_FS =  901; //tags a file in current directory
//codes for untagging files
const UTAG_FP = 1000;//untag file at specified path
const UTAG_FS = 1001;//untag file(s) in current directory
/////////////////////////////////////////////////////////
const CD_RLP =  1112; //changes directory (relative path)
const CD_ABS =  2222; //changes directory (absolute path)
const READ_XP=  3000; //read x bytes from file, given specified path
const READ_FP=  3300; // read whole file, given path
const READ_XCWD=3001; // read x bytes form file in current directory
const READ_FCWD=3002; // read whole file in current directory
const WRITE_FP= 4000; //writes to file, given a path
const APPEND_FP=4400; //append to a file, given a path
const WRITEXFPF=4440; //write x bytes from file to file,
const APPNDXFPF=4444; // append x bytes from file to file
const WRITEFCWD=4001; //write to file in current directory
const APPNDFCWD=4002; // append to file in current directory
const WRTXFCWDF=4003; // write x bytes from file to file in current directory
const APNDXFCWDF=4004; // append x bytes from file to file in current directory
const CPY_FP  = 6000; // copy contents of one file to another, givent he files paths
const CPY_FCWD= 6001; // Does the above, for the workign directory
const QUIT  =   999; //quit application
const FTL_ERR=  9999; // fatal error
const HNDSHK =  0;
const UHELP =   10001; //usage help
const UQUIT =   10002; //usage QUIT
const UFIND =   10003; // usage finding
const UNEW  =   10004; // usage finding
const UDEL  =   10005; //usage delete
const UOPEN =   10006; // Usage open
const UCLOSE=   10007; //usage close
const URNAME=   10008; //usage rename
const UATTR =   10009; //usage attributes
const UMERG =   10010; //usage merge
const UTAG  =   10011; //usage tags
const UUTAG =   10012; //usage untag
const UCD   =   10013; //usage chagne directory
const UREAD =   10014; //usage read
const UWRITE=   10015; //usage write

const LISATALL = 7000;
//const UWRITE=   10016; //usage write
/*------------------------------Static Interaction Variables------------------------------*/
////////////////////////////////////////////////////////////////////////////////////////////
/*-------------------------Code for Interacting with File system--------------------------*/
const daemonPort = 42069; //the port number for conencting to the file Systems
var host = 'localhost';
//add the requirements
var net = require('net');
var stream = require("stream");
//instantiate the net socket and buffer
var client = new net.Socket();
var buffer = Buffer.alloc(4096,'\0'); //buffer to size 4096 fill in with null bytes
//functions to interact with the tcp port
var currentOp = ""; //check what the current operation is for data return
var lastAr = [];  // keep data being added until next operation
var check = 0;
// callConnect function, will be called from outside this fille to establish conenction on start up
function onStartUp()
{
  var theVar = "HANDSHAKE-PartitionA";
  client.connect(daemonPort, host);
  buffer.writeIntBE(0);
  buffer.fill(theVar,4,4+theVar.length); //handshake
  client.write(buffer);
  buffer=Buffer.alloc(4096,'\0');
  sleep(500);
  getAllTags();
}
function connectTo()
{
  //alert("xxx");
  connection("301//newtest3333//$");
}
function getAllTags()
{
  //alert("get all tags");
  //client.connect(daemonPort, host);

  currentOp = '7000';

  buffer.write(currentOp); // writes the command to get all the tags TBD
  process.stdout.write(buffer);
  client.write(buffer); //writes the buffer to the client
  buffer = Buffer.alloc(4096,'\0');
  buffer.write("$");
  client.write(buffer); // signals that the communication is done
  buffer = Buffer.alloc(4096,'\0'); // reclears the buffer

}
function getFilesforTag(tag)
{
  process.stdout.write("getting files for system");
  currentOp = '400';
  buffer.write(currentOp);
  client.write(buffer);
  buffer = Buffer.alloc(4096,'\0');
  buffer.write(tag);
  client.write(buffer);
  buffer = Buffer.alloc(4096,'\0');
  buffer.write('$');
  client.write(buffer);
  buffer = Buffer.alloc(4096,'\0');
}
// communicates with the tcp server
function connection(request)
{
  //window.alert("callll");
//  alert("trying...");

  var inArray = request.split("//");
  currentOp = inArray[0];
  if(inArray[0]==='301') //for when a tag is being added
  {
    //alert(inArray.length);
      for(var i =0; i< inArray.length; i++)
      {
        process.stdout.write("Position: "+i+ " ");
        buffer.write(inArray[i]+'\n');
        process.stdout.write(buffer);
        client.write(buffer);
        buffer=Buffer.alloc(4096,'\0');
      }

  }
  lastAr = inArray;
}
//for when data is received
client.on('data', function(data)
{
  if(currentOp==='301') // checks if last op was to add a tag
  {
     //pop up alert to say that tag was added
    if(data.toString('utf8').includes("Created")) //verifies that the string was unique and creatable
    {
      alert(data);
      addToGrid('tag',lastAr[1]);
    }
    //else do nothing
  }

  else if(currentOp==='7000')// checks if the current op is the request all
  {
    if(check ===0)
    {
      check = 1;
    }
    else if(check ===1)
    {
      var templist = [];
      //alert(data);
      templist = data.toString().split('\n'); //split the data up
      setTagList(templist);
      int =0;
    }

  }
  else if(currentOp==='400') //searching for files based on tag
  {
    if(check ===0)
    {
      check = 1;
    }
    else if(check ===1)
    {
      var templist = [];
      var templistTwo = [];
      //alert(data);
      templist = data.toString().split('\n'); //split the data up
      for(var i = 1; i <templist.length; i++)
      {
        var forTemp = [];
        forTemp = templist[i].split('|');
        templistTwo.push(forTemp[0]);
      }
      setTagListFiles(templistTwo);
      int =0;
    }
  }

});
function close()
{
  client.end();
}
function sleep(miliseconds) //have a delay so that things don't execute too quickly
{
   var currentTime = new Date().getTime();

   while (currentTime + miliseconds >= new Date().getTime())
   {
   }
}
/*-------------------------Code for Interacting with File system--------------------------*/
