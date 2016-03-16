<?php
/**
 * @file
 * Formatters for CKAN datasets.
 */

/**
 * Implements hook_file_formatter_info().
 */
function govcms_ckan_media_file_formatter_info() {
  $formatters['govcms_ckan_media_image'] = array(
    'label' => t('CKAN Preview Image'),
    'file types' => array('ckan'),
    'default settings' => array(
      'image_style' => '',
    ),
    'view callback' => 'govcms_ckan_media_file_formatter_image_view',
    'mime types' => array('other/ckan'),
  );

  return $formatters;
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function govcms_ckan_media_file_formatter_image_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme == 'ckan') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    // Create a thumbnail placeholder from a static image.
    return array(
      '#theme' => 'image',
      '#path' => $wrapper->getThumbnailPath(),
      '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
    );
  }
}
