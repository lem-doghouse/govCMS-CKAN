<?php

/**
 * @file
 * Code for the govCMS CKAN module.
 */

/**
 * Create settings form for GovCms Ckan.
 *
 * @return mixed
 *   return admin form
 */
function govcms_ckan_settings_form() {

  $form = array();

  $form['govcms_ckan_endpoint_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint Url'),
    '#description' => t('Specify the endpoint url.'),
    '#weight' => 0,
    '#size' => 100,
    '#default_value' => variable_get('govcms_ckan_endpoint_url', '')
  );

  $form['govcms_ckan_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Api Key'),
    '#description' => t('Specify the API key.'),
    '#weight' => 1,
    '#size' => 100,
    '#default_value' => variable_get('govcms_ckan_api_key', '')
  );

  return system_settings_form($form);
}
