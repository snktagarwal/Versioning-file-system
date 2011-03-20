/* file: scalepage.js
   Ville-Petteri Makinen 2007 */

function scalePage() {

  // Find relevant elements.
  var bd = document.getElementById("bodyelement");
  var tb = document.getElementById("tabelement");
  var tu = document.getElementById("textunit");
  var screendim = Math.ceil(Math.sqrt(screen.height*screen.width));
  var textwidth = false;
  var rawsize = false;

  // Check that values are valid.
  if(tu != null) rawsize = Math.ceil(tu.offsetHeight);
  if(!screendim) screendim = 1000;
  if(!rawsize) rawsize = Math.ceil(2 + 0.012*screendim);

  // Determine text width.
  textwidth = Math.ceil(15*rawsize + 0.31*screendim);
  if(textwidth > 0.9*screendim) textwidth = Math.floor(0.9*screendim);
  if(textwidth < 0.3*screendim) textwidth = Math.ceil(0.3*screendim);

  // Determine font size.
  fontsize = Math.ceil(2 + textwidth/45);

  // Update page.
  if(tb) tb.setAttribute("width", (textwidth + "px"));
  var tables = document.getElementsByTagName("TABLE");
  for(i = 0; i < tables.length; i++)
    tables[i].style.fontSize = (fontsize + "px");
}
