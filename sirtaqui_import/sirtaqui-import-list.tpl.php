<ul>
<?php if (!empty($modules)) : ?>
	<?php foreach ($modules as $type => $module) : ?>
		<li>
			<span><?php print $type; ?></span>
			<a href="/admin/content/sirtaqui/import_list/import/<?php print $type; ?>/<?php print $module; ?>">Importer</a>
		</li>
	<?php endforeach; ?>
<?php endif; ?>
</ul>