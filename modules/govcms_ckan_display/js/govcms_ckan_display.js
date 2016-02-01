/**
 * GovCMS CKAN Display.
 */
(function ($){

  /**
   * Display charts based on selector passed from settings.
   */
  Drupal.behaviors.tableCharts = {
    attach: function (context, settings) {

      // Only auto add if we have settings.
      if (settings.govcms_ckan_display === undefined) {
        return;
      }

      // Tables to act on.
      var $tables = $(settings.govcms_ckan_display.tableChartSelectors.join(','), context);

      // Add tableCharts.
      $tables.once('table-charts').tableCharts();

    }
  };

})(jQuery);
