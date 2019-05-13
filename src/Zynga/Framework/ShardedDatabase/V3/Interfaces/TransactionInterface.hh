<?hh // strict

namespace Zynga\Framework\ShardedDatabase\V3\Interfaces;

use Zynga\Framework\ShardedDatabase\V3\Interfaces\DriverInterface;
use Zynga\Framework\Type\V1\Interfaces\TypeInterface;

interface TransactionInterface<TType as TypeInterface> {

  /**
   * Creates a database specific transaction object.
   * @param DriverInterface $driver
   * @return TransactionInterface
   */
  public function __construct(DriverInterface<TType> $driver);

  /**
   * Starts a transaction on your database.
   * @return bool
   */
  public function begin(): bool;

  /**
   * Commits the currently active transaction.
   * @return bool
   */
  public function commit(): bool;

  /**
   * Rolls the current transaction back on the database.
   * @return bool
   */
  public function rollback(): bool;

}