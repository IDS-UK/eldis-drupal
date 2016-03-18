<?php

/**
 * @file
 * Eldis jobs rules definitions.
 */

/**
 * Implements hook_rules_action_info_alter()
 *
 * We need to alter some default rules set by commerce_license
 * In order to deal with the
 */
function eldis_job_rules_action_info_alter(&$actions) {
  $actions['commerce_license_activate_order_licenses']['callbacks']['execute'] = 'eldis_job_activate_order_licenses';
}

/**
 * Implements hook_rules_event_info().
 *
 * Define some events that can be used to trigger actions
 * in the Rules UI.
 */
function eldis_job_rules_event_info() {
  $events = array();

  /*
  $events['eldis_license_sponsored_new_suspended'] = array(
    'label' => t('After suspending a newly created sponsored license'),
    'group' => t('Eldis job'),
    'variables' => array(
      'commerce_license' => array(
        'type' => 'commerce_license',
        'label' => t('The license'),
      ),
    ),
  );
*/
  return $events;
}

/**
 * Implements hook_rules_action_info().
 */
function eldis_job_rules_action_info() {
  $actions = array();

  $actions['eldis_job_license_revoke_pay_per_job'] = array(
    'label' => t('Revoke a "Pay per job" license'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('The node that we have just added.'),
      ),
    ),
    'group' => t('Eldis job'),
    'callbacks' => array(
      'execute' => 'eldis_job_license_revoke_pay_per_job',
    ),
  );

  /*
  $actions['eldis_job_license_prevent_duplicates'] = array(
    'label' => t('Check for duplicate licenses and redirect if found'), // be careful with this
    'parameter' => array(
      'commerce_line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('The line item that we are trying to add.'),
      ),
    ),
    'group' => t('Eldis job'),
    'callbacks' => array(
      'execute' => 'eldis_job_license_prevent_duplicates',
    ),
  );
*/
  return $actions;
}

/**
 * Rules action callback: overrides commerce_license_activate_order_licenses
 *
 * Places in exceptions for the Sponsored type, activate manually
 * following approval rather than automativally when order is completed
 *
 * @todo this gets executed twice due to the rules for free and
 * normal licenses... what's the best way to do this just once?
 * - this shouldn't actually happen once we are NOT paying 0
 * i.e. when free orders are properly handled, this will happen only once.
 */
function eldis_job_activate_order_licenses($order) {
  watchdog('eldis_job_activate_order_licenses', 'test');
  $licenses = commerce_license_get_order_licenses($order);
  foreach ($licenses as $license) {

    $license_wrapper = entity_metadata_wrapper('commerce_license', $license);
    $product = $license_wrapper->product->value();

    switch($product->sku) {
      case ELDIS_JOB_PLAN_SPONSORED_PRODUCT_SKU:
        watchdog('sponsored', 'sponsored');
        // @todo notify sponsored subscription is pending, to user
        // @todo notify pending sponsored application to review, to backoffice
        // @todo how to notify when license is actually activated, hook_entity_update()

        continue;

      case ELDIS_JOB_PLAN_PREMIUM_PAY_PER_POST_PRODUCT_SKU:
        watchdog('pay per job', 'pay per job');
        $license->activate();
        // @todo notify pay per post subscription activated

        break;
      case ELDIS_JOB_PLAN_PREMIUM_SUBSCRIPTION_PRODUCT_SKU:
        watchdog('premium', 'premium');
        $license->activate();
        // @todo notify premium subscription activated

        break;
    }
  }
}


/**
 * Rules action callback: Revoke context user's pay per job license.
 */
function eldis_job_license_revoke_pay_per_job($job) {

  // get the user from the job
  $user = user_load($job->uid);

  // return the users pay per job license, if exists. If not, move on.
  if($license = eldis_job_user_has_pay_per_job_license($user)) {
    // revoke the license
    commerce_license_revoke_action($license);
    // @todo notify that pay per job license has been used
    drupal_set_message('You have just posted your single "Pay per post" job. Please consider purchasing a monthly subscription.');
    watchdog('Revoke user pay per job license', $license->license_id);
  }
}


/**
 * Implements hook_commerce_line_item_presave().
 *
 * Prevent duplicates
 */
function _eldis_job_commerce_line_item_presave($line_item) {

  // This is not a license line item type, stop here.
  if (!in_array($line_item->type, commerce_license_line_item_types())) {
    return;
  }

  global $user;

  // we need to prevent duplicate line items
  // and prevent duplicate orders

  $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
  $product = $line_item_wrapper->commerce_product->value();

  /**
   * for sponsored
   * - prevent duplicate line item
   */


  // check if we already have an ACTIVE license for this product
  switch($product->sku) {
    case ELDIS_JOB_PLAN_PREMIUM_PAY_PER_POST_PRODUCT_SKU:
      if(eldis_jobs_non_revoked_license_exists($product, $user)) {
        drupal_set_message('Please create your single job before purchasing another Pay per Job License.');
        drupal_goto('jobs/recruiter/choosejobplan2');
      }
      break;

    case ELDIS_JOB_PLAN_SPONSORED_PRODUCT_SKU:
      // if we have an active sponsored sub
      if(eldis_jobs_non_revoked_license_exists($product, $user)) {
        drupal_set_message('You currently have an active Sponsored subscription, please post some jobs!');
        drupal_goto('jobs/recruiter/choosejobplan');
      }
      // if we have a PENDING sponsored sub
      if(eldis_jobs_non_revoked_license_exists($product, $user)) { // note we use a custom function that finds a license of either active or created state
        drupal_set_message('You currently have an Sponsored subscription application that is pending review...');
        eldis_job_empty_cart(); // remove the items that we've just added
        drupal_goto('jobs/recruiter/choosejobplan');
      }
      break;

    case ELDIS_JOB_PLAN_PREMIUM_SUBSCRIPTION_PRODUCT_SKU:
      if(eldis_jobs_non_revoked_license_exists($product, $user)) {

        print('we have license');
        exit;
        drupal_set_message('You currently have an active Monthly subscription, please post some jobs!');
        drupal_goto('jobs/recruiter/choosejobplan');
      }
      break;

    default:
      // unknown sku!

  }
}
