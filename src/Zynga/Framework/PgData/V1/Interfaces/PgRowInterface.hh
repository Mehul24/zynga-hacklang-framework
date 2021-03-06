<?hh // strict

namespace Zynga\Framework\PgData\V1\Interfaces;

use Zynga\Framework\StorableObject\V1\Interfaces\StorableObjectInterface;
use Zynga\Framework\Type\V1\Interfaces\TypeInterface;

interface PgRowInterface extends StorableObjectInterface {
  public function pgModel(): PgModelInterface;
  public function getPrimaryKeyIsFromDatabase(): bool;
  public function getPrimaryKeyNextValue(): TypeInterface;
  public function getPrimaryKey(): string;
  public function getPrimaryKeyTyped(): TypeInterface;
  public function getTableName(): string;
  public function save(): bool;
  public function delete(): bool;
}
