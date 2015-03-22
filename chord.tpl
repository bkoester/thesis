<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      #tooltip {
        color: white;
        opacity: .9;
        background: #333;
        padding: 5px;
        border: 1px solid lightgrey;
        border-radius: 5px;
        position: absolute;
        z-index: 10;
        visibility: hidden;
        white-space: nowrap;
        pointer-events: none;
      }
      #circle circle {
        fill: none;
        pointer-events: all;
      }
      path.group {
        fill-opacity: .8;
      }
      path.chord {
        fill-opacity: .8;
        stroke: #000;
        stroke-width: .25px;
      }
      #circle:hover path.fade {
        display: none;
      }
    </style>
  </head>
  <body>
    <div id="tooltip"></div>
    <script src="/static/node_modules/d3/d3.js"></script>
    <script src="/static/node_modules/underscore/underscore.js"></script>
    <script src="/static/js/mapper.js"></script>
    <script>
      //*******************************************************************
      //  CREATE MATRIX AND MAP
      //*******************************************************************
      d3.tsv('/data/network_{{field}}_{{weightedness}}.tsv',
			  function (error, data) {
        var mpr = chordMpr(data);

        mpr
          .addValuesToMap('{{field}}1')
          .addValuesToMap('{{field}}2')
          .setFilter(function (row, a, b) {
            return ((row.{{field}}1 === a.name && row.{{field}}2 === b.name) ||
				(row.{{field}}1 === b.name && row.{{field}}2 === a.name))
          })
          .setAccessor(function (recs, a, b) {
            if (!recs[0]) return 0;
            return +recs[0].count;
          });
        drawChords(mpr.getMatrix(), mpr.getMap());
      });
      //*******************************************************************
      //  DRAW THE CHORD DIAGRAM
      //*******************************************************************
      function drawChords (matrix, mmap) {
        var w = 980, h = 800, r1 = h / 2, r0 = r1 - 100;

        var fill = d3.scale.ordinal()
            .domain(d3.range(4))
            .range(["#000000", "#FFDD89", "#957244", "#F26223"]);

        var chord = d3.layout.chord()
            .padding(.02)
            .sortSubgroups(d3.descending)
            .sortChords(d3.descending);

        var arc = d3.svg.arc()
            .innerRadius(r0)
            .outerRadius(r0 + 20);

        var svg = d3.select("body").append("svg:svg")
            .attr("width", w)
            .attr("height", h)
          .append("svg:g")
            .attr("id", "circle")
            .attr("transform", "translate(" + w / 2 + "," + h / 2 + ")");

            svg.append("circle")
                .attr("r", r0 + 20);

        var rdr = chordRdr(matrix, mmap);
        chord.matrix(matrix);

        var g = svg.selectAll("g.group")
            .data(chord.groups())
          .enter().append("svg:g")
            .attr("class", "group")
            .on("mouseover", mouseover)
            .on("mouseout", function (d) { d3.select("#tooltip").style("visibility", "hidden") });

        g.append("svg:path")
            .style("stroke", "black")
            .style("fill", function(d) { return fill(d.index); })
            .attr("d", arc);

        g.append("svg:text")
            .each(function(d) { d.angle = (d.startAngle + d.endAngle) / 2; })
            .attr("dy", ".35em")
            .style("font-family", "helvetica, arial, sans-serif")
            .style("font-size", "10px")
            .attr("text-anchor", function(d) { return d.angle > Math.PI ? "end" : null; })
            .attr("transform", function(d) {
              return "rotate(" + (d.angle * 180 / Math.PI - 90) + ")"
                  + "translate(" + (r0 + 26) + ")"
                  + (d.angle > Math.PI ? "rotate(180)" : "");
            })
            .text(function(d) { return rdr(d).gname; });

          var chordPaths = svg.selectAll("path.chord")
                .data(chord.chords())
              .enter().append("svg:path")
                .attr("class", "chord")
                .style("stroke", function(d) { return d3.rgb(fill(d.target.index)).darker(); })
                .style("fill", function(d) { return fill(d.target.index); })
                .attr("d", d3.svg.chord().radius(r0))
                .on("mouseover", function (d) {
                  d3.select("#tooltip")
                    .style("visibility", "visible")
                    .html(chordTip(rdr(d)))
                    .style("top", function () { return (d3.event.pageY - 100)+"px"})
                    .style("left", function () { return (d3.event.pageX - 100)+"px";})
                })
                .on("mouseout", function (d) { d3.select("#tooltip").style("visibility", "hidden") });

          function chordTip (d) {
            var p = d3.format(".2%"), q = d3.format(",.3r")
            return "Chord Info:<br/>"
              + p(d.svalue/d.stotal) + " (" + q(d.svalue) + ") of "
              + d.sname + " to " + d.tname
          }

          function groupTip (d) {
            var p = d3.format(".1%"), q = d3.format(",.3r")
            return "Group Info:<br/>"
                + d.gname + " : " + q(d.gvalue) + "<br/>"
                + p(d.gvalue/d.mtotal) + " of Matrix Total (" + q(d.mtotal) + ")"
          }

          function mouseover(d, i) {
            d3.select("#tooltip")
              .style("visibility", "visible")
              .html(groupTip(rdr(d)))
              .style("top", function () { return (d3.event.pageY - 80)+"px"})
              .style("left", function () { return (d3.event.pageX - 130)+"px";})

            chordPaths.classed("fade", function(p) {
              return p.source.index != i
                  && p.target.index != i;
            });
          }
      }

    </script>
  </body>
</html>
