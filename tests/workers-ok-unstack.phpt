--TEST--
Test pthreads Worker::unstack
--DESCRIPTION--
This test verifies that unstack functions as intended
--FILE--
<?php

$worker = new Worker();

$worker->stack(new class extends Threaded implements Collectable {
	public function run() {
		var_dump($this);
	}
	public function isGarbage() : bool { return true; }
});

var_dump($worker->unstack());

$worker->shutdown();
?>
--EXPECTF--
object(class@anonymous)#%d (%d) {
}
