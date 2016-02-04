<?php
/**
 * @file
 * A temporary demonstration page. Will be removed once integration complete.
 */

/**
 * Demo page callback.
 */
function govcms_ckan_demo_page() {
  $client = govcms_ckan_client();
  $response = $client->get('action/datastore_search', array('id' => '531eb12d-4c60-4cdc-a26e-e500188c0d33'));

  return 'GovCMS CKAN Demo';
}
