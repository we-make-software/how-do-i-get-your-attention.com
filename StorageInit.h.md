# StorageInit.h

The `StorageInit.h` file provides three external functions, `StorageInit()`, `StorageExit()`, and `CentralProcessingUnitTaskGetStorageSize()`, which are utilized in `wms.c`.

- **`StorageInit()`** is responsible for initializing the storage system.
- **`StorageExit()`** is responsible for cleaning up and exiting the storage system.
- **`CentralProcessingUnitTaskGetStorageSize()`** returns the total size of the disk used by the CPU task, helping to manage read/write operations.

These functions are only required within `wms.c`.

---

In the future, it’s essential to recall how to manage disks for **wms** using the `dd` command effectively. The disk image stored at `/root/wms.disk` is used for **wms**, and here’s a step-by-step guide on how to handle copying partitions into this disk image.

To copy data from a specific sector of a partition and skip unnecessary blocks, we use the following `dd` command with `sparse`:

```bash
nohup sudo dd if=/dev/sdb4 of=/root/wms.disk bs=512 skip=11718656 conv=sparse & disown
```

- **`skip=11718656`**: Skips the first 11,718,656 sectors, starting the copy from sector 11,718,657 onward.
- **`conv=sparse`**: Creates a sparse file by not writing zero blocks, saving space.
- **`nohup` & `disown`**: Ensures the process continues running in the background even after closing the terminal.

In the future, you may need to add additional partitions to the same disk image file (`/root/wms.disk`). This is easily done by using the `dd` command with appropriate options.

- **Copy the contents of `/dev/sda1` to the disk image**:

```bash
sudo dd if=/dev/sda1 of=/root/wms.disk bs=512 count=<count_of_sda1> conv=sparse
```

- **Append `/dev/sda2` to the disk image** without overwriting existing data:

```bash
sudo dd if=/dev/sda2 of=/root/wms.disk bs=512 count=<count_of_sda2> oflag=append conv=notrunc sparse
```

- **`count=<count_of_sda1/sda2>`**: Specifies the number of blocks to copy from the partition.
- **`oflag=append`**: Ensures that new data is appended to the existing disk image rather than overwriting it.
- **`conv=notrunc`**: Prevents truncation of the existing file content, ensuring all current data remains intact.

If you are running multiple `dd` processes or other tasks at the same time, remember to monitor system resources carefully. Use tools like `top` and `ionice` to adjust CPU and I/O priorities to prevent `dd` from consuming too much CPU or disk bandwidth. This ensures that **wms** runs smoothly even while disk operations are in progress.

```bash
sudo renice 19 <dd_process_pid>  # Lower CPU priority of the dd process
sudo ionice -c3 -p <dd_process_pid>  # Lower I/O priority of the dd process
```

Lowering priorities ensures that disk operations don’t interfere with **wms**’s performance.
