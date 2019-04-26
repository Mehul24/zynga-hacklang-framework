<?hh // strict

namespace Zynga\Framework\Cache\V2\Driver;

use Zynga\Framework\StorableObject\V1\Interfaces\StorableObjectInterface;

use Zynga\Framework\Dynamic\V1\DynamicClassCreation;

use Zynga\Framework\Cache\V2\Driver\Base as DriverBase;
use Zynga\Framework\Cache\V2\Interfaces\DriverConfigInterface;
use Zynga\Framework\Cache\V2\Interfaces\DriverInterface;
use Zynga\Framework\Cache\V2\Exceptions\NoServerPairsProvidedException;
use Zynga\Framework\Cache\V2\Exceptions\NoConnectionException;
use Zynga\Framework\Cache\V2\Exceptions\StorableObjectRequiredException;

use Zynga\Framework\Exception\V1\Exception;

use \Memcache as NativeMemcacheDriver;

class Memcache extends DriverBase {
  private NativeMemcacheDriver $_memcache;
  private DriverConfigInterface $_config;

  public function __construct(DriverConfigInterface $config) {
    $this->_config = $config;
    $this->_memcache = new NativeMemcacheDriver();
  }

  public function getConfig(): DriverConfigInterface {
    return $this->_config;
  }

  public function connect(): bool {

    // --
    // sadly they don't provide a way to see if the connection has been
    // configured. So you jiggle getversion to see if there is anything
    // configured. SAD PANDA!
    // --
    $versionInfo = $this->_memcache->getversion();

    if ($this->_memcache->getversion() !== false) {
      return true;
    }

    $memcache = $this->_memcache;

    try {

      $serverPairs = $this->getConfig()->getServerPairings();

      if ($serverPairs->count() == 0) {
        throw new NoServerPairsProvidedException(
          'config='.get_class($this->getConfig()),
        );
      }

      // add the host / port combinations to the memcache object, addserver
      //   always returns true as it lazy connects at use time.
      foreach ($serverPairs as $host => $port) {
        $memcache->addserver($host, $port);
      }

      $this->_memcache = $memcache;

      return true;

    } catch (Exception $e) {
      throw $e;
    }

  }

  private function getKeySupportingOverride(
    StorableObjectInterface $obj,
    string $keyOverride,
  ): string {

    $key = $keyOverride;

    if ($keyOverride == '') {
      $key = $this->getConfig()->createKeyFromStorableObject($obj);
    }

    return $key;

  }

  public function add(
    StorableObjectInterface $obj,
    string $keyOverride = '',
  ): bool {

    try {

      $key = $this->getKeySupportingOverride($obj, $keyOverride);

      $this->connect();

      $return = $this->_memcache->add($key, $obj);

      if ($return == true) {
        return true;
      }

      return false;

    } catch (Exception $e) {
      throw $e;
    }

  }

  public function get(
    StorableObjectInterface $obj,
    string $keyOverride = '',
  ): ?StorableObjectInterface {

    try {

      $key = $this->getKeySupportingOverride($obj, $keyOverride);

      $this->connect();

      $data = $this->_memcache->get($key);

      // no data to work with.
      if ($data === false) {
        return null;
      }

      $obj->import()->fromJSON($data);

      return $obj;

    } catch (Exception $e) {
      throw $e;
    }

  }

  public function set(
    StorableObjectInterface $obj,
    string $keyOverride = '',
  ): bool {

    try {

      $key = $this->getKeySupportingOverride($obj, $keyOverride);

      $this->connect();

      $flags = 0;

      $ttl = time() + $this->getConfig()->getTTL();

      $success =
        $this->_memcache->set($key, $obj->export()->asJSON(), $flags, $ttl);

      return $success;

    } catch (Exception $e) {
      throw $e;
    }

  }

  public function delete(
    StorableObjectInterface $obj,
    string $keyOverride = '',
  ): bool {

    try {

      $key = $this->getKeySupportingOverride($obj, $keyOverride);

      $this->connect();

      $success = $this->_memcache->delete($key);

      return $success;

    } catch (Exception $e) {
      throw $e;
    }

  }

}
