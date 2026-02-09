// Definitions to manipulate the IDT from userspace
//
// This is _very_ safe! Users should be able to touch
// this stuff!

// Here's a cool opportunity to learn about the IDT!
// https://wiki.osdev.org/Interrupt_Descriptor_Table
//
// Dr. Gheith described that it's kinda messy, (and it
// is a little bit messy...), but in x86-32, its just 8
// bytes with these values
//
// We only really care about the offset here:
// that is, off_hi << 16 | off_lo -- this gives us
// the address of the corresponding handler
pub struct IDTEntry {
    off_lo: u16,
    seg_sel: u16,
    flags: u16,
    off_hi: u16,
}

// And then the IDT itself is just an array of these
// entries.
//
// It's got a size of 256 on x86-32, but we don't really
// care about that. We only care where it starts!
pub struct IDT {
    base: *const IDTEntry,
}

impl IDT {
    pub fn new(base: *const IDTEntry) -> Self {
        IDT { base }
    }

    fn print_raw(&self, entry: &IDTEntry) {
        println!(
            "Raw: \n\t{:x} \n\t{:x} \n\t{:x} \n\t{:x}",
            entry.off_lo, entry.seg_sel, entry.flags, entry.off_hi
        );
    }

    pub fn address(&self, idx: usize) -> u32 {
        unsafe {
            let entry: *const u32 = self.base.add(idx) as *const u32;
            entry as u32
        }
    }

    pub fn contents(&self, idx: usize) {
        println!("Contents of Address: {:x}", self.address(idx));
        let entry: &IDTEntry = unsafe { &*self.base.add(idx) };
        println!(
            "IDT Entry {}: {{ off_lo: {:x}, flags: {:x}, seg_sel: {:x}, off_hi: {:x} }}",
            idx, entry.off_lo, entry.flags, entry.seg_sel, entry.off_hi
        );
        self.print_raw(entry);
    }

    // NOTE: This probably doesn't work?
    pub fn create(&self, idx: usize, handler: u32, flags: u16, s_cpy_idx: usize) {
        println!("Registering IDT Entry at Address: {:x}", self.address(idx));
        let entry: *mut IDTEntry = unsafe { self.base.add(idx) as *mut IDTEntry };
        let s_cpy_entry: &IDTEntry = unsafe { &*self.base.add(s_cpy_idx) };
        let off_lo: u16 = (handler & 0xFFFF) as u16;
        let off_hi: u16 = ((handler >> 16) & 0xFFFF) as u16;
        let seg_sel: u16 = s_cpy_entry.seg_sel; // this is why
        unsafe {
            (*entry).off_lo = off_lo;
            (*entry).seg_sel = seg_sel;
            (*entry).flags = flags;
            (*entry).off_hi = off_hi;
        }
        self.print_raw(unsafe { &*entry });
    }

    pub fn modify(&self, idx: usize, n_off: u32) {
        println!("Modifying Address: {:x}", self.address(idx));
        let entry: *mut IDTEntry = unsafe { self.base.add(idx) as *mut IDTEntry };
        let off_lo: u16 = (n_off & 0xFFFF) as u16;
        let off_hi: u16 = ((n_off >> 16) & 0xFFFF) as u16;
        println!(
            "n_off: {:x}, n_off_lo: {:x}, n_off_hi {:x}",
            n_off, off_lo, off_hi
        );
        unsafe {
            (*entry).off_lo = off_lo;
            (*entry).off_hi = off_hi;
            (*entry).seg_sel = USS;
        };
        self.print_raw(unsafe { &*entry });
    }
}

// This is the address given by the MBR (hopefully)
//
// This is where it's placed for me... It'd be cool if it
// were mostly the same for others
pub const IDT_BASE: *const IDTEntry = 0x87c0 as *const IDTEntry;

// These are a few important entries we may want
// to take a looksie at...
pub const PIT: usize = 40;
pub const INT: usize = 48;
pub const VIR: usize = 27; // :3

// We may also need to hack at the CS?
pub const USS: u16 = 32 + 3; // RPL hack
