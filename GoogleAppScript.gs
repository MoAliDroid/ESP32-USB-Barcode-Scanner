function doPost(e) {
  var data = e.parameter.data;
  var out = e.parameter.out;
  var stage = e.parameter.stage;

  // Extract prodID from the first 4 digits of the data
  var prodID = data.substring(0, 4);

  var timestamp = Utilities.formatDate(new Date(), "GMT+3", "yyyy/MM/dd hh:mm:ss a");    //date and time
  
  var sheetName;

  // Determine the sheet name based on stage and out variables
  if (stage == 1 && out == "false") {sheetName = 'Stage1_in';}
  else if (stage == 1 && out == "true") {sheetName = 'Stage1_out';}
  else if (stage == 2 && out == "false") {sheetName = 'Stage2_in';}
  else if (stage == 2 && out == "true") {sheetName = 'Stage2_out';}
  else if (stage == 3 && out == "false") {sheetName = 'Stage3_in';}
  else if (stage == 3 && out == "true") {sheetName = 'Stage3_out';}
  else {
    // If stage or out values are not as expected, handle accordingly
    return ContentService.createTextOutput("Invalid stage or out value").setMimeType(ContentService.MimeType.TEXT);
  }

  var sheet = SpreadsheetApp.openById('YOUR_SPREADSHEET_ID_HERE').getSheetByName(sheetName); // Get the appropriate sheet based on stage and out

  // Append prodID, data, stage, and timestamp in the same row
  sheet.appendRow([prodID, data, stage, timestamp]); 

  return ContentService.createTextOutput("Success").setMimeType(ContentService.MimeType.TEXT);
}