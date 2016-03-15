<?php
/**
 * @file
 * Extends the MediaReadOnlyStreamWrapper class to handle CKAN datasets.
 */

/**
 * MediaGovCmsCkanStreamWrapper class.
 *
 *  Create an instance like this:
 *  $dataset = new MediaYouTubeStreamWrapper('ckan://dataset');
 */
class MediaGovCmsCkanStreamWrapper extends MediaReadOnlyStreamWrapper {

  /**
   * MediaGovCmsCkanStreamWrapper constructor.
   */
  public function __construct() {
    // TODO: Fix this so it is a correct public url.
    $this->base_url = variable_get('govcms_ckan_endpoint_url', '');
  }

  /**
   * Define the mime type.
   */
  public static function getMimeType($uri, $mapping = NULL) {
    return 'other/ckan';
  }

  /**
   * Get the thumbnail for a graph.
   */
  public function getThumbnailPath() {
    return drupal_get_path('module', 'govcms_ckan_media') . '/images/thumbnail.png';
  }

}
