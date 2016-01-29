(function ($) {

  /**
   * Update the summary for the module's vertical tab.
   */
  Drupal.behaviors.govcms_ckanFieldsetSummaries = {
    attach: function (context) {
      // Use the fieldset class to identify the vertical tab element
      $('.govcms-ckan-graph-node-fieldset-tab', context).drupalSetSummary(function (context) {
        // Depending on the checkbox status, the settings will be customized, so
        // update the summary with the custom setting textfield string or a use a
        // default string.
        if ($('#edit-govcms-ckan-graph-tab-enabled', context).attr('checked')) {
          return Drupal.checkPlain($('#edit-ggovcms-ckan-graph-tab-custom-setting', context).val());

        }
        else {
          return Drupal.t('Using default');
        }
      });
    }
  };

})(jQuery);
