<?php
// $Id: content-admin-display-overview-form.tpl.php,v 1.1.2.3 2008/10/09 20:58:26 karens Exp $
?>

<div>
  <?php print $help; ?>
</div>
<?php if ($rows): ?>
  <table id="content-display-overview" class="sticky-enabled">
    <thead>
      <tr>
        <th><?php print t('Field'); ?></th>
        <?php if ($basic): ?>
          <th><?php print t('Label'); ?></th>
        <?php endif; ?>
        <?php foreach ($contexts as $key => $value): ?>
          <th><?php print $value['title']; ?></th>
          <th><?php print t('Exclude'); ?></th>
        <?php endforeach; ?>
      </tr>
    </thead>
    <tbody>

        <tr>
            <td class="region"><strong><?php print(t("Batch settings")); ?></strong></td>

            <?php if ($basic): ?>
                <td class="region"><?php print(drupal_render($helpers['label_helper'])); ?></td>
            <?php endif; ?>

            <?php foreach ($contexts as $context => $title): ?>
                <td class="region"><?php print(drupal_render($helpers[$context . '_display_helper'])); ?></td>
                <td class="region"><?php print(drupal_render($helpers[$context . '_exclude_helper'])); ?></td>
            <?php endforeach; ?>
        </tr>

      <?php
      $count = 0;
      foreach ($rows as $row): ?>
        <tr class="<?php print $count % 2 == 0 ? 'odd' : 'even'; ?>">
          <td><?php print $row->indentation; ?><span class="<?php print $row->label_class; ?>"><?php print $row->human_name; ?></span></td>
          <?php if ($basic): ?>
            <?php $classes = isset($row->label_helper_class) ? 'label_helper ' . $row->label_helper_class : '' ?>
            <td class="<?php print($classes); ?>"><?php print $row->label; ?></td>
          <?php endif; ?>
          <?php foreach ($contexts as $context => $title): ?>
                <?php $classes = isset($row->display_helper_class) ? $context . '_' . 'display_helper ' . $context . '_' . $row->display_helper_class : ''; ?>
                <td class="<?php print($classes); ?>"><?php print $row->{$context}->format; ?></td>

                <?php $classes = isset($row->exclude_helper_class) ? $context . '_' . 'exclude_helper ' . $context . '_' . $row->exclude_helper_class : ''; ?>
                <td class="<?php print($classes); ?>"><?php print $row->{$context}->exclude; ?></td>
          <?php endforeach; ?>
        </tr>
        <?php $count++;
      endforeach; ?>
    </tbody>
  </table>
  <?php print $submit; ?>
<?php endif; ?>
