<?hh // strict

namespace Zynga\Framework\StorableObject\Collections\Vector\V1\Exporter;

use
  Zynga\Framework\StorableObject\Collections\V1\Interfaces\StorableCollection
;

use Zynga\Framework\StorableObject\V1\Interfaces\ExportInterface;
use Zynga\Framework\StorableObject\V1\Interfaces\StorableObjectInterface;

use Zynga\Framework\StorableObject\V1\Fields\Generic as FieldsGeneric;
use Zynga\Framework\StorableObject\V1\Exceptions\NoFieldsFoundException;
use
  Zynga\Framework\StorableObject\V1\Exceptions\OperationNotSupportedException
;

use Zynga\Framework\Type\V1\Interfaces\TypeInterface;

use Zynga\Framework\Exception\V1\Exception;

/**
 * Base class for collection exporters. This class should be inherited
 * and each collection should have their own concrete exporter.
 */
class Base<Tv> implements ExportInterface {

  public function __construct(private StorableCollection<Tv> $collection) {}

  public function asJSON(
    ?string $parentFieldName = null,
    bool $sorted = false,
  ): string {
    try {

      $stack = '';

      if ($parentFieldName !== null) {
        $stack .= json_encode($parentFieldName).':';
      }

      // --
      // We serialize the individual items off into a json array, no need to make
      // a temporary array and then run json encode on it as the vector / array
      // type is so simplistic and the child storable objects will handle the
      // heavy lifting of serialization.
      // --

      $stack .= '[';

      $firstItem = true;

      $items = $this->collection->items();

      foreach ($items as $storableItem) {

        // We can skip the value if the item is still in default state.
        list($isRequired, $isDefaultValue) =
          FieldsGeneric::getIsRequiredAndIsDefaultValue($storableItem);

        if ($isDefaultValue === true) {
          continue;
        }

        $value = null;
        if ($storableItem instanceof StorableObjectInterface) {
          try {
            $value = $storableItem->export()->asJSON(null, $sorted);
          } catch (Exception $e) {
            throw $e;
          }
        } else if ($storableItem instanceof TypeInterface) {
          $value = json_encode($storableItem->get());
        }

        if ($value !== null) {
          if ($firstItem === false) {
            $stack .= ',';
          }
          $stack .= $value;
          $firstItem = false;
        }
      }

      $stack .= ']';
      return $stack;

    } catch (Exception $e) {
      throw $e;
    }
  }

  public function asArray(): array<string, mixed> {
    try {

      $map = array();

      $fieldNum = 0;
      $items = $this->collection->items();

      foreach ($items as $storableItem) {
        $fieldName = "".$fieldNum;

        // We can skip the value if the item is still in default state.
        list($isRequired, $isDefaultValue) =
          FieldsGeneric::getIsRequiredAndIsDefaultValue($storableItem);

        if ($isDefaultValue === true) {
          continue;
        }

        $value = null;
        if ($storableItem instanceof StorableObjectInterface) {
          try {
            $value = $storableItem->export()->asArray();
          } catch (Exception $e) {
            throw $e;
          }
        } else if ($storableItem instanceof TypeInterface) {
          $value = $storableItem->get();
        }

        if ($value !== null) {
          $map[$fieldName] = $value;
          $fieldNum++;
        }
      }

      return $map;

    } catch (Exception $e) {
      throw $e;
    }
  }

  /**
   * This function returns the values in the collection as a map, recursively
   * Since this collection doesn't have field names, we use incrementing numerals for keys,
   * The same way the base Map class does when importing an array
   */
  public function asMap(): Map<string, mixed> {
    try {

      $map = Map {};

      $fieldNum = 0;
      $items = $this->collection->items();

      foreach ($items as $storableItem) {
        $fieldName = "".$fieldNum;

        // We can skip the value if the item is still in default state.
        list($isRequired, $isDefaultValue) =
          FieldsGeneric::getIsRequiredAndIsDefaultValue($storableItem);

        if ($isDefaultValue === true) {
          continue;
        }

        $value = null;
        if ($storableItem instanceof StorableObjectInterface) {
          try {
            $value = $storableItem->export()->asMap();
          } catch (Exception $e) {
            throw $e;
          }
        } else if ($storableItem instanceof TypeInterface) {
          $value = $storableItem->get();
        }

        if ($value !== null) {
          $map->set($fieldName, $value);
          $fieldNum++;
        }
      }

      return $map;

    } catch (Exception $e) {
      throw $e;
    }
  }

  public function asBinary(): string {
    throw new OperationNotSupportedException(__METHOD__.' not supported');
  }

}
