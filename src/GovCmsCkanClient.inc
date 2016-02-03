<?php
/**
 * @file
 * GovCmsCkanClient Class for fetching, caching and returning ckan data.
 */

/**
 * Defines the GovCMS CKAN Client class.
 */
class GovCmsCkanClient {

  private $apiUrl = 'http://data.gov.au/api/3';
  private $apiKey = '256a71bd-2f09-4a48-87bb-fb7af0cc51b7';

  /**
   * Set up the fetch class.
   */
  public function __construct($info, $params) {
    $this->getSettings();
  }

  /**
   * Return the data.
   */
  public function get($resource, $id) {

  }

  /**
   * Populate the settings using drupal variables.
   */
  private function getSettings() {

  }

  /**
   * Fetch CKAN data.
   */
  private function fetch() {

  }

  /**
   * Parse the raw CKAN data.
   */
  private function parseData() {

  }

  /**
   * Caches remote data locally.
   */
  private function cacheData() {

  }

}
