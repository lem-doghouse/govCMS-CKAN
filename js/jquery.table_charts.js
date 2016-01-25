(function ($) {

  /*
   * tableCharts implementation.
   *
   * This is a jQuery helper that handles turning data outputted in tablular
   * format into charts.
   */

  /*
   * Storage for all the charts on the page.
   */
  var tableCharts = [];

  /*
   * tableChart class.
   *
   * @param dom
   *   The dom for a single table.
   */
  var tableChart = function(dom, settings) {
    var self = this;

    // The dom of the table.
    self.dom = dom;
    self.$dom = $(dom);

    // Available options and defaults.
    self.defaults = {
      chartId: 0,
      type: 'line',
      rotated: false,
      data: {},
      toggleText: 'Show {view}',
      chartViewName: 'chart',
      tableViewName: 'table',
      defaultView: 'chart', // This must be either chartViewName or tableViewName.
      dataAttributes: ['type', 'rotated'],
      component: 'table-chart'
    };

    // Settings start with defaults and extended by options passed to the constructor.
    self.settings = $.extend(self.defaults, settings);

    // Store the current view
    self.currentView = self.settings.defaultView;

    /*
     * Parse settings from the table attributes.
     * @see settings.dataAttributes for what gets parsed.
     */
    self.parseSettings = function() {
      var val = null;

      // Available settings are found in the dataAttributes setting. We loop through
      // each of those and if not empty, override the settings.
      $(self.settings.dataAttributes).each(function(i, attr) {
        val = self.$dom.data(attr);
        if (val !== null) {
          self.settings[attr] = val;
        }
      });
    };

    /*
     * Parse the data from the table into an array suitable for c3js/d3js.
     *
     * NOTE: Assumes that all tables have a TH and this is the label for
     * that data set. All other values in that column are assumed to be integers.
     */
    self.parseData = function() {
      var columns = [], val, $cell;

      // On each row.
      $('tr', self.$dom).each(function(r, row) {

        // On each cell.
        $('th,td', row).each(function(c, cell) {
          // Create our data set if doesn't exists yet.
          columns[c] = columns[c] !== undefined ? columns[c] : [];

          // Cells should be an Int unless it is the header.
          $cell = $(cell);
          // TODO: investigate why .prop('tagName') isn't working here.
          val = $cell[0].tagName === 'TH' ? $cell.html() : parseInt($cell.html());

          // Add the rows to the correct data set.
          columns[c].push(val);
        });

      });

      // Add to settings.
      self.settings.data = {
        columns: columns
      }
    };

    /*
     * Helper to create toggle button text.
     */
    self.toggleButtonText = function(view) {
      return self.settings.toggleText.replace('{view}', view);
    };

    /*
     * Toggle visibility of the chart and table.
     */
    self.toggleView = function() {
      // TODO: Consider doing this with css classes?
      self.$chart.toggle();
      self.$dom.toggle();
      // Update button text.
      self.$toggle.html(self.toggleButtonText(self.currentView));
      // Update current view.
      self.currentView = self.currentView == self.settings.chartViewName ? self.settings.tableViewName : self.settings.chartViewName;
    };

    /*
     * Prepare the markup on the page for charts.
     */
    self.buildMarkup = function() {
      // Build our chart dom ID, and dom elements we'll need.
      self.chartDomId = 'table-chart-' + self.settings.chartId;
      self.$chart = $('<div>');
      self.$toggle = $('<button>');

      // Give the table a unique class.
      self.$dom.addClass(self.settings.component + '--table');

      // Add a toggle button after the table.
      self.$toggle.html(self.toggleButtonText('table'))
        .addClass(self.settings.component + '--toggle')
        .click(self.toggleView)
        .insertAfter(self.$dom);

      // Add chart placeholder to dom with a unique Id.
      self.$chart.attr('id', self.chartDomId)
        .addClass(self.settings.component + '--chart')
        .insertAfter(self.$dom);

      // Hide the table.
      self.$dom.hide();

      // Build the chart.
      // TODO: Move to promise/callback?
      self.buildChart();
    };

    /*
     * Build the chart.
     */
    self.buildChart = function() {
      // TODO: this will handle callbacks for different graph types.
      // Currently just a basic POC.

      // Specify the type of chart.
      self.settings.data.type = self.settings.type;

      // Basic c3 chart.
      self.chart = c3.generate({
        bindto: '#' + self.chartDomId,
        data: self.settings.data,
        axis: {
          rotated: self.settings.rotated
        }
      })
    };

    /*
     * Initialize the class.
     */
    self.init = function() {
      self.parseSettings();
      self.parseData();
      self.buildMarkup();
    };

    // Init on construct.
    self.init();

  };

  /*
   * jQuery plugin/function.
   */
  $.fn.tableCharts = function (settings) {
    window.tableCharts = window.tableCharts || [];
    return this.each(function (i, dom) {
      // Store all the charts on the page in tableCharts.
      // Each chart needs a unique ID for the page.
      // TODO: Consider alternative way of creating a chartId, will get conflicts if called multiple times.
      window.tableCharts.push(
        new tableChart(dom, {chartId: i})
      );
    });
  };

  /*
   * ---------------------------
   * TESTING ONLY (example page).
   * ---------------------------
   */
  $(document).ready(function(){
    $(".table-chart").tableCharts();
  })

})(jQuery);