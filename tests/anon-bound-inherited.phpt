--TEST--
Test anonymous classes (bound inherited class)
--DESCRIPTION--mae
This test verifies that anonymous Threaded objects work as expected
--FILE--
<?php
$worker = new Worker();

$collectable = new class extends Collectable {
	public function run() {
		var_dump($this instanceof Collectable);	
	}
};

$worker->start();

$worker->stack($collectable);

$worker->shutdown();
--EXPECT--
bool(true)
