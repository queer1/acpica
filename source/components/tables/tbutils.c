/******************************************************************************
 *
 * Module Name: tbutils - ACPI Table utilities
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999 - 2014, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights. You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code. No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision. In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change. Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee. Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution. In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE. ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT, ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES. INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS. INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES. THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government. In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#define __TBUTILS_C__

#include "acpi.h"
#include "accommon.h"
#include "actables.h"

#define _COMPONENT          ACPI_TABLES
        ACPI_MODULE_NAME    ("tbutils")


/* Local prototypes */

static ACPI_STATUS
AcpiTbValidateXsdt (
    ACPI_PHYSICAL_ADDRESS   Address);

static ACPI_PHYSICAL_ADDRESS
AcpiTbGetRootTableEntry (
    UINT8                   *TableEntry,
    UINT32                  TableEntrySize);


#if (!ACPI_REDUCED_HARDWARE)
/*******************************************************************************
 *
 * FUNCTION:    AcpiTbInitializeFacs
 *
 * PARAMETERS:  None
 *
 * RETURN:      Status
 *
 * DESCRIPTION: Create a permanent mapping for the FADT and save it in a global
 *              for accessing the Global Lock and Firmware Waking Vector
 *
 ******************************************************************************/

ACPI_STATUS
AcpiTbInitializeFacs (
    void)
{
    ACPI_STATUS             Status;


    /* If Hardware Reduced flag is set, there is no FACS */

    if (AcpiGbl_ReducedHardware)
    {
        AcpiGbl_FACS = NULL;
        return (AE_OK);
    }

    Status = AcpiGetTableByIndex (ACPI_TABLE_INDEX_FACS,
                ACPI_CAST_INDIRECT_PTR (ACPI_TABLE_HEADER, &AcpiGbl_FACS));
    return (Status);
}
#endif /* !ACPI_REDUCED_HARDWARE */


/*******************************************************************************
 *
 * FUNCTION:    AcpiTbTablesLoaded
 *
 * PARAMETERS:  None
 *
 * RETURN:      TRUE if required ACPI tables are loaded
 *
 * DESCRIPTION: Determine if the minimum required ACPI tables are present
 *              (FADT, FACS, DSDT)
 *
 ******************************************************************************/

BOOLEAN
AcpiTbTablesLoaded (
    void)
{

    if (AcpiGbl_RootTableList.CurrentTableCount >= 3)
    {
        return (TRUE);
    }

    return (FALSE);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiTbCheckDsdtHeader
 *
 * PARAMETERS:  None
 *
 * RETURN:      None
 *
 * DESCRIPTION: Quick compare to check validity of the DSDT. This will detect
 *              if the DSDT has been replaced from outside the OS and/or if
 *              the DSDT header has been corrupted.
 *
 ******************************************************************************/

void
AcpiTbCheckDsdtHeader (
    void)
{

    /* Compare original length and checksum to current values */

    if (AcpiGbl_OriginalDsdtHeader.Length != AcpiGbl_DSDT->Length ||
        AcpiGbl_OriginalDsdtHeader.Checksum != AcpiGbl_DSDT->Checksum)
    {
        ACPI_BIOS_ERROR ((AE_INFO,
            "The DSDT has been corrupted or replaced - "
            "old, new headers below"));
        AcpiTbPrintTableHeader (0, &AcpiGbl_OriginalDsdtHeader);
        AcpiTbPrintTableHeader (0, AcpiGbl_DSDT);

        /* Disable further error messages */

        AcpiGbl_OriginalDsdtHeader.Length = AcpiGbl_DSDT->Length;
        AcpiGbl_OriginalDsdtHeader.Checksum = AcpiGbl_DSDT->Checksum;
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiTbCopyDsdt
 *
 * PARAMETERS:  TableDesc           - Installed table to copy
 *
 * RETURN:      None
 *
 * DESCRIPTION: Implements a subsystem option to copy the DSDT to local memory.
 *              Some very bad BIOSs are known to either corrupt the DSDT or
 *              install a new, bad DSDT. This copy works around the problem.
 *
 ******************************************************************************/

ACPI_TABLE_HEADER *
AcpiTbCopyDsdt (
    UINT32                  TableIndex)
{
    ACPI_TABLE_HEADER       *NewTable;
    ACPI_TABLE_DESC         *TableDesc;


    TableDesc = &AcpiGbl_RootTableList.Tables[TableIndex];

    NewTable = ACPI_ALLOCATE (TableDesc->Length);
    if (!NewTable)
    {
        ACPI_ERROR ((AE_INFO, "Could not copy DSDT of length 0x%X",
            TableDesc->Length));
        return (NULL);
    }

    ACPI_MEMCPY (NewTable, TableDesc->Pointer, TableDesc->Length);
    AcpiTbDeleteTable (TableDesc);
    TableDesc->Pointer = NewTable;
    TableDesc->Flags = ACPI_TABLE_ORIGIN_ALLOCATED;

    ACPI_INFO ((AE_INFO,
        "Forced DSDT copy: length 0x%05X copied locally, original unmapped",
        NewTable->Length));

    return (NewTable);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiTbInstallTable
 *
 * PARAMETERS:  Address                 - Physical address of DSDT or FACS
 *              Signature               - Table signature, NULL if no need to
 *                                        match
 *              TableIndex              - Index into root table array
 *
 * RETURN:      None
 *
 * DESCRIPTION: Install an ACPI table into the global data structure. The
 *              table override mechanism is called to allow the host
 *              OS to replace any table before it is installed in the root
 *              table array.
 *
 ******************************************************************************/

void
AcpiTbInstallTable (
    ACPI_PHYSICAL_ADDRESS   Address,
    char                    *Signature,
    UINT32                  TableIndex)
{
    ACPI_TABLE_HEADER       *Table;
    ACPI_TABLE_HEADER       *FinalTable;
    ACPI_TABLE_DESC         *TableDesc;


    if (!Address)
    {
        ACPI_ERROR ((AE_INFO, "Null physical address for ACPI table [%s]",
            Signature));
        return;
    }

    /* Map just the table header */

    Table = AcpiOsMapMemory (Address, sizeof (ACPI_TABLE_HEADER));
    if (!Table)
    {
        ACPI_ERROR ((AE_INFO, "Could not map memory for table [%s] at %p",
            Signature, ACPI_CAST_PTR (void, Address)));
        return;
    }

    /* If a particular signature is expected (DSDT/FACS), it must match */

    if (Signature &&
        !ACPI_COMPARE_NAME (Table->Signature, Signature))
    {
        ACPI_BIOS_ERROR ((AE_INFO,
            "Invalid signature 0x%X for ACPI table, expected [%s]",
            *ACPI_CAST_PTR (UINT32, Table->Signature), Signature));
        goto UnmapAndExit;
    }

    /*
     * Initialize the table entry. Set the pointer to NULL, since the
     * table is not fully mapped at this time.
     */
    TableDesc = &AcpiGbl_RootTableList.Tables[TableIndex];

    TableDesc->Address = Address;
    TableDesc->Pointer = NULL;
    TableDesc->Length = Table->Length;
    TableDesc->Flags = ACPI_TABLE_ORIGIN_MAPPED;
    ACPI_MOVE_32_TO_32 (TableDesc->Signature.Ascii, Table->Signature);

    /*
     * ACPI Table Override:
     *
     * Before we install the table, let the host OS override it with a new
     * one if desired. Any table within the RSDT/XSDT can be replaced,
     * including the DSDT which is pointed to by the FADT.
     *
     * NOTE: If the table is overridden, then FinalTable will contain a
     * mapped pointer to the full new table. If the table is not overridden,
     * or if there has been a physical override, then the table will be
     * fully mapped later (in verify table). In any case, we must
     * unmap the header that was mapped above.
     */
    FinalTable = AcpiTbTableOverride (Table, TableDesc);
    if (!FinalTable)
    {
        FinalTable = Table; /* There was no override */
    }

    AcpiTbPrintTableHeader (TableDesc->Address, FinalTable);

    /* Set the global integer width (based upon revision of the DSDT) */

    if (TableIndex == ACPI_TABLE_INDEX_DSDT)
    {
        AcpiUtSetIntegerWidth (FinalTable->Revision);
    }

    /*
     * If we have a physical override during this early loading of the ACPI
     * tables, unmap the table for now. It will be mapped again later when
     * it is actually used. This supports very early loading of ACPI tables,
     * before virtual memory is fully initialized and running within the
     * host OS. Note: A logical override has the ACPI_TABLE_ORIGIN_OVERRIDE
     * flag set and will not be deleted below.
     */
    if (FinalTable != Table)
    {
        AcpiTbDeleteTable (TableDesc);
    }


UnmapAndExit:

    /* Always unmap the table header that we mapped above */

    AcpiOsUnmapMemory (Table, sizeof (ACPI_TABLE_HEADER));
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiTbGetRootTableEntry
 *
 * PARAMETERS:  TableEntry          - Pointer to the RSDT/XSDT table entry
 *              TableEntrySize      - sizeof 32 or 64 (RSDT or XSDT)
 *
 * RETURN:      Physical address extracted from the root table
 *
 * DESCRIPTION: Get one root table entry. Handles 32-bit and 64-bit cases on
 *              both 32-bit and 64-bit platforms
 *
 * NOTE:        ACPI_PHYSICAL_ADDRESS is 32-bit on 32-bit platforms, 64-bit on
 *              64-bit platforms.
 *
 ******************************************************************************/

static ACPI_PHYSICAL_ADDRESS
AcpiTbGetRootTableEntry (
    UINT8                   *TableEntry,
    UINT32                  TableEntrySize)
{
    UINT64                  Address64;


    /*
     * Get the table physical address (32-bit for RSDT, 64-bit for XSDT):
     * Note: Addresses are 32-bit aligned (not 64) in both RSDT and XSDT
     */
    if (TableEntrySize == ACPI_RSDT_ENTRY_SIZE)
    {
        /*
         * 32-bit platform, RSDT: Return 32-bit table entry
         * 64-bit platform, RSDT: Expand 32-bit to 64-bit and return
         */
        return ((ACPI_PHYSICAL_ADDRESS) (*ACPI_CAST_PTR (UINT32, TableEntry)));
    }
    else
    {
        /*
         * 32-bit platform, XSDT: Truncate 64-bit to 32-bit and return
         * 64-bit platform, XSDT: Move (unaligned) 64-bit to local,
         *  return 64-bit
         */
        ACPI_MOVE_64_TO_64 (&Address64, TableEntry);

#if ACPI_MACHINE_WIDTH == 32
        if (Address64 > ACPI_UINT32_MAX)
        {
            /* Will truncate 64-bit address to 32 bits, issue warning */

            ACPI_BIOS_WARNING ((AE_INFO,
                "64-bit Physical Address in XSDT is too large (0x%8.8X%8.8X),"
                " truncating",
                ACPI_FORMAT_UINT64 (Address64)));
        }
#endif
        return ((ACPI_PHYSICAL_ADDRESS) (Address64));
    }
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiTbValidateXsdt
 *
 * PARAMETERS:  Address             - Physical address of the XSDT (from RSDP)
 *
 * RETURN:      Status. AE_OK if the table appears to be valid.
 *
 * DESCRIPTION: Validate an XSDT to ensure that it is of minimum size and does
 *              not contain any NULL entries. A problem that is seen in the
 *              field is that the XSDT exists, but is actually useless because
 *              of one or more (or all) NULL entries.
 *
 ******************************************************************************/

static ACPI_STATUS
AcpiTbValidateXsdt (
    ACPI_PHYSICAL_ADDRESS   XsdtAddress)
{
    ACPI_TABLE_HEADER       *Table;
    UINT8                   *NextEntry;
    ACPI_PHYSICAL_ADDRESS   Address;
    UINT32                  Length;
    UINT32                  EntryCount;
    ACPI_STATUS             Status;
    UINT32                  i;


    /* Get the XSDT length */

    Table = AcpiOsMapMemory (XsdtAddress, sizeof (ACPI_TABLE_HEADER));
    if (!Table)
    {
        return (AE_NO_MEMORY);
    }

    Length = Table->Length;
    AcpiOsUnmapMemory (Table, sizeof (ACPI_TABLE_HEADER));

    /*
     * Minimum XSDT length is the size of the standard ACPI header
     * plus one physical address entry
     */
    if (Length < (sizeof (ACPI_TABLE_HEADER) + ACPI_XSDT_ENTRY_SIZE))
    {
        return (AE_INVALID_TABLE_LENGTH);
    }

    /* Map the entire XSDT */

    Table = AcpiOsMapMemory (XsdtAddress, Length);
    if (!Table)
    {
        return (AE_NO_MEMORY);
    }

    /* Get the number of entries and pointer to first entry */

    Status = AE_OK;
    NextEntry = ACPI_ADD_PTR (UINT8, Table, sizeof (ACPI_TABLE_HEADER));
    EntryCount = (UINT32) ((Table->Length - sizeof (ACPI_TABLE_HEADER)) /
        ACPI_XSDT_ENTRY_SIZE);

    /* Validate each entry (physical address) within the XSDT */

    for (i = 0; i < EntryCount; i++)
    {
        Address = AcpiTbGetRootTableEntry (NextEntry, ACPI_XSDT_ENTRY_SIZE);
        if (!Address)
        {
            /* Detected a NULL entry, XSDT is invalid */

            Status = AE_NULL_ENTRY;
            break;
        }

        NextEntry += ACPI_XSDT_ENTRY_SIZE;
    }

    /* Unmap table */

    AcpiOsUnmapMemory (Table, Length);
    return (Status);
}


/*******************************************************************************
 *
 * FUNCTION:    AcpiTbParseRootTable
 *
 * PARAMETERS:  Rsdp                    - Pointer to the RSDP
 *
 * RETURN:      Status
 *
 * DESCRIPTION: This function is called to parse the Root System Description
 *              Table (RSDT or XSDT)
 *
 * NOTE:        Tables are mapped (not copied) for efficiency. The FACS must
 *              be mapped and cannot be copied because it contains the actual
 *              memory location of the ACPI Global Lock.
 *
 ******************************************************************************/

ACPI_STATUS
AcpiTbParseRootTable (
    ACPI_PHYSICAL_ADDRESS   RsdpAddress)
{
    ACPI_TABLE_RSDP         *Rsdp;
    UINT32                  TableEntrySize;
    UINT32                  i;
    UINT32                  TableCount;
    ACPI_TABLE_HEADER       *Table;
    ACPI_PHYSICAL_ADDRESS   Address;
    UINT32                  Length;
    UINT8                   *TableEntry;
    ACPI_STATUS             Status;


    ACPI_FUNCTION_TRACE (TbParseRootTable);


    /* Map the entire RSDP and extract the address of the RSDT or XSDT */

    Rsdp = AcpiOsMapMemory (RsdpAddress, sizeof (ACPI_TABLE_RSDP));
    if (!Rsdp)
    {
        return_ACPI_STATUS (AE_NO_MEMORY);
    }

    AcpiTbPrintTableHeader (RsdpAddress,
        ACPI_CAST_PTR (ACPI_TABLE_HEADER, Rsdp));

    /* Use XSDT if present and not overridden. Otherwise, use RSDT */

    if ((Rsdp->Revision > 1) &&
        Rsdp->XsdtPhysicalAddress &&
        !AcpiGbl_DoNotUseXsdt)
    {
        /*
         * RSDP contains an XSDT (64-bit physical addresses). We must use
         * the XSDT if the revision is > 1 and the XSDT pointer is present,
         * as per the ACPI specification.
         */
        Address = (ACPI_PHYSICAL_ADDRESS) Rsdp->XsdtPhysicalAddress;
        TableEntrySize = ACPI_XSDT_ENTRY_SIZE;
    }
    else
    {
        /* Root table is an RSDT (32-bit physical addresses) */

        Address = (ACPI_PHYSICAL_ADDRESS) Rsdp->RsdtPhysicalAddress;
        TableEntrySize = ACPI_RSDT_ENTRY_SIZE;
    }

    /*
     * It is not possible to map more than one entry in some environments,
     * so unmap the RSDP here before mapping other tables
     */
    AcpiOsUnmapMemory (Rsdp, sizeof (ACPI_TABLE_RSDP));

    /*
     * If it is present and used, validate the XSDT for access/size
     * and ensure that all table entries are at least non-NULL
     */
    if (TableEntrySize == ACPI_XSDT_ENTRY_SIZE)
    {
        Status = AcpiTbValidateXsdt (Address);
        if (ACPI_FAILURE (Status))
        {
            ACPI_BIOS_WARNING ((AE_INFO, "XSDT is invalid (%s), using RSDT",
                AcpiFormatException (Status)));

            /* Fall back to the RSDT */

            Address = (ACPI_PHYSICAL_ADDRESS) Rsdp->RsdtPhysicalAddress;
            TableEntrySize = ACPI_RSDT_ENTRY_SIZE;
        }
    }

    /* Map the RSDT/XSDT table header to get the full table length */

    Table = AcpiOsMapMemory (Address, sizeof (ACPI_TABLE_HEADER));
    if (!Table)
    {
        return_ACPI_STATUS (AE_NO_MEMORY);
    }

    AcpiTbPrintTableHeader (Address, Table);

    /*
     * Validate length of the table, and map entire table.
     * Minimum length table must contain at least one entry.
     */
    Length = Table->Length;
    AcpiOsUnmapMemory (Table, sizeof (ACPI_TABLE_HEADER));

    if (Length < (sizeof (ACPI_TABLE_HEADER) + TableEntrySize))
    {
        ACPI_BIOS_ERROR ((AE_INFO,
            "Invalid table length 0x%X in RSDT/XSDT", Length));
        return_ACPI_STATUS (AE_INVALID_TABLE_LENGTH);
    }

    Table = AcpiOsMapMemory (Address, Length);
    if (!Table)
    {
        return_ACPI_STATUS (AE_NO_MEMORY);
    }

    /* Validate the root table checksum */

    Status = AcpiTbVerifyChecksum (Table, Length);
    if (ACPI_FAILURE (Status))
    {
        AcpiOsUnmapMemory (Table, Length);
        return_ACPI_STATUS (Status);
    }

    /* Get the number of entries and pointer to first entry */

    TableCount = (UINT32) ((Table->Length - sizeof (ACPI_TABLE_HEADER)) /
        TableEntrySize);
    TableEntry = ACPI_ADD_PTR (UINT8, Table, sizeof (ACPI_TABLE_HEADER));

    /*
     * First two entries in the table array are reserved for the DSDT
     * and FACS, which are not actually present in the RSDT/XSDT - they
     * come from the FADT
     */
    AcpiGbl_RootTableList.CurrentTableCount = 2;

    /* Initialize the root table array from the RSDT/XSDT */

    for (i = 0; i < TableCount; i++)
    {
        if (AcpiGbl_RootTableList.CurrentTableCount >=
            AcpiGbl_RootTableList.MaxTableCount)
        {
            /* There is no more room in the root table array, attempt resize */

            Status = AcpiTbResizeRootTableList ();
            if (ACPI_FAILURE (Status))
            {
                ACPI_WARNING ((AE_INFO, "Truncating %u table entries!",
                    (unsigned) (TableCount -
                    (AcpiGbl_RootTableList.CurrentTableCount - 2))));
                break;
            }
        }

        /* Get the table physical address (32-bit for RSDT, 64-bit for XSDT) */

        AcpiGbl_RootTableList.Tables[AcpiGbl_RootTableList.CurrentTableCount].Address =
            AcpiTbGetRootTableEntry (TableEntry, TableEntrySize);

        TableEntry += TableEntrySize;
        AcpiGbl_RootTableList.CurrentTableCount++;
    }

    /*
     * It is not possible to map more than one entry in some environments,
     * so unmap the root table here before mapping other tables
     */
    AcpiOsUnmapMemory (Table, Length);

    /*
     * Complete the initialization of the root table array by examining
     * the header of each table
     */
    for (i = 2; i < AcpiGbl_RootTableList.CurrentTableCount; i++)
    {
        AcpiTbInstallTable (AcpiGbl_RootTableList.Tables[i].Address,
            NULL, i);

        /* Special case for FADT - validate it then get the DSDT and FACS */

        if (ACPI_COMPARE_NAME (
                &AcpiGbl_RootTableList.Tables[i].Signature, ACPI_SIG_FADT))
        {
            AcpiTbParseFadt (i);
        }
    }

    return_ACPI_STATUS (AE_OK);
}
