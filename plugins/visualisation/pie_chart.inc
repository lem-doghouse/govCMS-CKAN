<?php
$plugin = array(
  'title' => t('Pie chart visualisation'),
);

/**
 * Returns a renderable array that represents the block content
 */
function govcms_ckan_pie_chart_view() {
  return 'This will be a renderable array. We will also load up our js using a \'#attached\' property';

}

function govcms_ckan_pie_chart_options(&$plugin, $info) {
  return array(
    'options' => array(
      'enable' => array(
        'type' => 'checkbox',
        'title' => 'Enable this visualisation',
        'default_value' => 0
      )
    )
  );
}