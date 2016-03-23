<?php

/**
 * @file
 * Default rule configurations for License.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function eldis_job_default_rules_configuration() {
  $rules = array();

  // Add a reaction rule to revoke the license of a pay per job user, after initial job creation
  $rule = rules_reaction_rule();
  $rule->label = t('After adding a job, if the user has a pay per job license, revoke it');
  $rule->tags = array('Eldis job');
  $rule->active = TRUE;
  $rule->weight = 9;
  $rule
    ->event('node_insert')
    ->condition('data_is', array('data:select' => 'node:type', 'value' => 'job'))
    ->action('eldis_job_license_revoke_pay_per_job', array(
      'data:select' => 'node',
    ));

  $rules['eldis_job_license_revoke_after_add_single_job'] = $rule;

  return $rules;
}
