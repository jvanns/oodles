#ifndef OODLES_LINKER_HPP
#define OODLES_LINKER_HPP

namespace oodles {

struct Link; // Forward declaration for Linker

class Linker
{
    public:
        /* Member functions/methods */
        Linker();
        ~Linker();
    protected:
        /* Member variables/attributes */
        Link *coupling;
        
        /* Member functions/methods */
        bool extend_link_to(Linker *l);
    private:
        /* Member functions/methods */
        void unlink();
        void link(Link *l);

        /* Friend class declarations */
        friend class Link; /// Link wants link() only
};

struct Link
{
    /* Member functions/methods */
    Link(Linker *left, Linker *right);
    Link(const Link &l);
    
    Link& operator= (const Link &l);
    Linker* complement_of(const Linker &source) const;
    
    /* Member variables/attributes */
    Linker *left, *right;
};

} // oodles

#endif
