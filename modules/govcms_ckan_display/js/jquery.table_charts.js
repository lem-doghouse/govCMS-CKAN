(function ($) {

  "use strict";

  /*
   * TableCharts jQuery plugin.
   * --------------------------
   * This is a class + jQuery plugin that handles turning data outputted in tabular
   * format into a standardised settings set ready to be used with charts.
   *
   * Usage.
   * ------
   * Call this on the table you want to create a chart for:
   * `$('.table-selector').tableCharts()`
   * Defaults are listed below in the tableChart class.
   *
   * Settings for the charts obtained from the dom, they can also optionally be passed to this class.
   * - Settings stored as attributes in the table element
   * -- data-type: The type of chart in use (eg. line, spline, bar, stacked, area, area-spline).
   * -- data-chart: The chart implementation to use (see below)
   * -- data-rotated: Default is false, change to true to rotate the table axis.
   * -- data-labels: Should lables be shown at each data point. Defaults to false
   * -- data-defaultView: Should the chart or table be displayed first. Defaults to 'chart'.
   * -- data-palette: A comma separated list of hex colours to be used for the palette.
   * -- data-grid: Grid lines to use: xy, x or y
   * -- data-xLabel: The optional label to show on the X axis
   * -- data-yLabel: The optional label to show on the Y axis
   * - Table headings (th) is used as the label and the following attributes can be used
   * -- data-color: Hex colour, alternative to using palette on the table element.
   * -- data-style: The style for the line (dashed, solid)
   * - Each column forms a data set, Table headings can be strings but table values must be Ints.
   *
   * Chart Implementations.
   * ----------------------
   * Chart implementations are used to turn the settings from tableChart into a chart.
   * They are abstracted from tableChart to allow flexibility for different chart types.
   *
   * To add a new chart implementation, add a function to the tableChartsChart object,
   * it will get passed the settings parsed by tableChart.
   *
   * c3js implementation is included in this file. @see tableChartsChart.c3js
   */

  /*
   * Storage for all the charts on the page.
   */
  var tableCharts = [];

  /*
   * Storage for the chart classes available.
   */
  var tableChartsChart = {};

  /**
   * tableChart class.
   *
   * Handles the parsing and dom transformation of a table and hands settings over to
   * the selected tableChartsChart class to render the chart.
   *
   * @param dom
   *   The dom for a single table.
   */
  var TableChart = function(dom, settings) {
    var self = this;

    /*
     * Available options and defaults.
     */
    self.defaults = {
      // The table element dom.
      dom: dom,
      $dom: $(dom),
      // A unique Id for this chart.
      chartId: 0,
      // The dom Id for the chart element.
      chartDomId: 'table-chart-0',
      // The type of chart.
      type: 'line',
      // The tableChartChart class used to create the chart.
      chart: 'c3js',
      // Chart settings.
      rotated: false,
      palette: [],
      labels: false,
      styles: [],
      grid: null,
      xLabel: null,
      yLabel: null,
      stacked: false,
      // The data for the chart.
      columns: [],
      data: {},
      group: [],
      // The labels to show on the x axis ticks.
      xLabels: [],
      // Data attributes automatically parsed from the table element.
      dataAttributes: ['type', 'rotated', 'labels', 'defaultView', 'grid', 'xLabel', 'yLabel', 'stacked'],
      // Chart views determine what is displaying chart vs table.
      chartViewName: 'chart',
      tableViewName: 'table',
      // defaultView must be either chartViewName or tableViewName.
      defaultView: 'chart',
      // The text for the toggle button {view} gets replaced with the view name.
      toggleText: 'Show {view}',
      // Component prefix used for dom classes and ids.
      component: 'table-chart',
      // Chart Initialized callback
      chartInitCallback: function () {}
    };

    // Settings start with defaults and extended by options passed to the constructor.
    self.settings = $.extend(self.defaults, settings);

    // Store the current view
    self.currentView = self.settings.defaultView;

    // Define the chartDomId based on the chartId.
    self.settings.chartDomId = self.settings.component + '-' + self.settings.chartId;

    /*
     * Parse settings from the table attributes.
     * @see settings.dataAttributes for what gets parsed.
     */
    self.parseSettings = function () {
      var val = null;

      // Available settings are found in the dataAttributes setting. We loop through
      // each of those and if not empty, override the settings.
      $(self.settings.dataAttributes).each(function (i, attr) {
        val = self.settings.$dom.data(attr);
        if (val !== undefined && val !== null) {
          self.settings[attr] = val;
        }
      });

      // Palette gets transformed into an array (may be overridden by table headings).
      if (self.settings.$dom.data('palette') !== undefined) {
        self.settings.palette = self.settings.$dom.data('palette').replace(' ', '').split(',');
      }

      // Return self for chaining.
      return self;
    };

    /*
     * Parse additional settings from table headings.
     */
    self.parseTableHeading = function(col, $th) {
      // Override colour for this data set.
      if ($th.data('color') !== undefined) {
        self.settings.palette[col] = $th.data('color');
      }

      // Currently only style option is dashed and will only work with line.
      // @see http://c3js.org/samples/simple_regions.html
      if ($th.data('style') !== undefined && $th.data('style') === 'dashed') {
        self.settings.styles.push({set: $th.html(), style: $th.data('style')});
      }

      // Create a group of headings (used for stacking).
      self.settings.group.push($th.html());

      // Return self for chaining.
      return self;
    };

    /*
     * Parse the cells to see if they are a X label cell.
      *
     * @param $cell
     *   Table cell jQuery object.
     *
     * @returns {boolean}
     *   true if is a X label, false if not.
     */
    self.parseXLabel = function($cell) {
      if ($cell.data('xLabel') !== undefined && $cell.data('xLabel')) {
        // Header cell gets th axis being used (x) otherwise label is the cell html.
        var label = $cell.is('th') ? 'x' : $cell.html();
        self.settings.xLabels.push(label);
        return true;
      }
      return false;
    };

    /*
     * Parse the data from the table into an array suitable for c3js/d3js.
     *
     * NOTE: Assumes that all tables have a TH and this is the label for
     * that data set. All other values in that column are assumed to be integers.
     */
    self.parseData = function () {
      var columns = [], val, $cell, col;

      // On each row.
      $('tr', self.settings.$dom).each(function (r, row) {
        col = 0;

        // On each cell.
        $('th,td', row).each(function (c, cell) {
          $cell = $(cell);

          // If a X label we abort don't parse this cell any further.
          if (self.parseXLabel($cell) === true) {
            return;
          }

          // Create our data set if doesn't exists yet.
          columns[col] = columns[col] !== undefined ? columns[col] : [];

          // If dealing with the table headers.
          if ($cell.is('th')) {
            val = $cell.html();
            self.parseTableHeading(c, $cell);
          } else {
            // Cell values should always be an Int.
            val = parseInt($cell.html());
          }

          // Add the rows to the correct data set.
          columns[col].push(val);
          col++;

        });

      });

      // Add parsed data rows to settings.
      self.settings.columns = columns;

      // Return self for chaining.
      return self;
    };

    /*
     * Helper to create toggle button text.
     *
     * @return string
     *   The text for a toggle button based on the state.
     */
    self.toggleButtonText = function (view) {
      var s = self.settings;
      if (view === undefined) {
        // If no view specified, use the opposite of what the defaultView is.
        view = s.defaultView === s.chartViewName ? s.tableViewName : s.chartViewName;
      }
      return s.toggleText.replace('{view}', view);
    };

    /*
     * Toggle visibility of the chart and table.
     */
    self.toggleView = function () {
      self.$chart.toggle();
      self.settings.$dom.toggle();
      // Update button text.
      self.$toggle.html(self.toggleButtonText(self.currentView));
      // Update current view.
      self.currentView = self.currentView === self.settings.chartViewName ? self.settings.tableViewName : self.settings.chartViewName;

      // Return self for chaining.
      return self;
    };

    /*
     * Prepare the markup on the page for charts.
     */
    self.buildMarkup = function () {
      // Build our chart dom ID, and dom elements we'll need.
      self.$chart = $('<div>');
      self.$toggle = $('<button>');

      // Give the table a unique class.
      self.settings.$dom.addClass(self.settings.component + '--table');

      // Add a toggle button after the table.
      self.$toggle.html(self.toggleButtonText())
        .addClass(self.settings.component + '--toggle')
        .click(self.toggleView)
        .insertAfter(self.settings.$dom);

      // Add chart placeholder to dom with a unique Id.
      self.$chart.attr('id', self.settings.chartDomId)
        .addClass(self.settings.component + '--chart')
        .insertAfter(self.settings.$dom);

      // Display only table or chart depending on defaultView.
      if (self.settings.defaultView === self.settings.chartViewName) {
        self.settings.$dom.hide();
      } else {
        self.$chart.hide();
      }

      self
        // Now the markup is ready, build the chart.
        .buildChart()
        // Add download buttons. TODO: Move elsewhere.
        .addDownloadButtons();

      // Return self for chaining.
      return self;
    };

    /*
     * Build the chart based on the chart setting.
     */
    self.buildChart = function () {
      // If a chart implementation exists, call it and pass the settings.
      if (typeof tableChartsChart[self.settings.chart] === 'function') {
        tableChartsChart[self.settings.chart](self.settings);
      } else {
        // No implementation found.
        self.$chart.html('No chart implementation found for ' + self.settings.chart);
      }

      // Return self for chaining.
      return self;
    };

    /*
     * Add download buttons, requires $.chartExport()
     *
     * TODO: Move to a more appropriate place! Should not live in this class.
     */
    self.addDownloadButtons = function() {
      if (typeof $.fn.chartExport !== 'function') {
        return;
      }
      var buttonTypes = ['svg', 'png'];
      $(buttonTypes).each(function (i, format) {
        $('<button>')
          .html('Download as ' + format)
          .insertAfter(self.$toggle)
          .addClass(self.settings.component + '--download')
          .chartExport({format: format, svg: self.$chart});
      });

      // Return self for chaining.
      return self;
    };

    /*
     * Initialize the class.
     */
    self.init = function () {
      self
        .parseSettings()
        .parseData()
        .buildMarkup();
    };

    // Init on construct.
    self.init();

    // Return self for chaining.
    return self;
  };

  /**
   * The tableCharts c3js implementation.
   *
   * @param settings
   *   The parsed settings from tableCharts.
   *
   * TODO: If this gets to large, move to its own file.
   */
  tableChartsChart.c3js = function (settings) {
    // Ensure library is loaded.
    if (typeof c3 === 'undefined') {
      alert('c3js library not found');
      return;
    }

    // Type of chart is stored in the data.
    settings.data.type = settings.type;

    // Placeholder for the data columns.
    settings.data.columns = [];

    // Stacked can be applied to most charts, the stack order used is
    // the column order.
    if (settings.stacked) {
      settings.data.groups = [settings.group];
    }

    // Apply styles (currently only works with lines and dashes)
    if (settings.styles.length) {
      settings.data.regions = {};
      $(settings.styles).each(function (i, d){
        settings.data.regions[d.set] = [{style: d.style}];
      });
    }

    // Define the axis settings.
    var axis = {
      rotated: settings.rotated,
      x: {label: settings.xLabel},
      y: {label: settings.yLabel}
    };

    // Add X axis labels.
    if (settings.xLabels.length) {
      settings.data.x = 'x';
      settings.data.columns.push(settings.xLabels);
      // this needed to load string x value
      axis.x.type = 'category';
    };

    // Show labels on data points?
    settings.data.labels = settings.labels;

    // Add the data columns.
    $(settings.columns).each(function (i, col) {
      console.log(col);
      settings.data.columns.push(col);
    });

    // Options structure to be passed to c3
    var options = {
      bindto: '#' + settings.chartDomId,
      data: settings.data,
      axis: axis,
      color: {
        pattern: settings.palette
      },
      oninit: settings.chartInitCallback
    };

    // Add optional grid lines.
    switch (settings.grid) {
      case 'xy':
        options.grid = {x: {show: true}, y: {show: true}};
        break;
      case 'x':
        options.grid = {x: {show: true}};
        break;
      case 'y':
        options.grid = {y: {show: true}};
        break;
    }

    // Create chart.
    c3.generate(options);
  };

  /**
   * jQuery plugin/function.
   */
  $.fn.tableCharts = function (settings) {
    window.tableCharts = window.tableCharts || [];
    settings = settings || {};
    return this.each(function (i, dom) {
      // Store all the charts on the page in tableCharts.
      // Each chart needs a unique ID for the page.
      // TODO: Consider alternative way of creating a chartId, will get conflicts if called multiple times.
      settings.chartId = i;
      window.tableCharts.push(
        new TableChart(dom, settings)
      );
    });
  };

})(jQuery);
