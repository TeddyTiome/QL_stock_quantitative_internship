#include "include/qlSim.h"
namespace
{
class alpha83: public AlphaBase
{
    public:
    explicit alpha83(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vwap(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_vwap")),
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")), 
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),        
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        Num1(cfg->getAttributeIntDefault("para1",5)),
		Num2(cfg->getAttributeIntDefault("para2",2))
    {
        vol_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vol_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }          

    }
    void generate(int di) override
    {		
       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vol_rank[di-delay][ii] =vol[di-delay][ii];
            }
        }

        QL_Oputils::rank(vol_rank[di-delay]);

    		vector<float> rank1(GLOBAL::Instruments.size(),nan);
            vector<float> rank2(GLOBAL::Instruments.size(),nan);

		
        for (int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii){

        	if((valid[ di ][ ii ]))
            {
            	vector<float> c1(Num1,nan);
                float f1;

			for (int i= 0; i<Num1; ++i){
				c1[i]=close[di-delay-Num2-i][ii];
			}
				f1 = float(QL_Oputils::mean(c1));

				if (!(GLOBALFUNC::equal(f1,0))){
				rank1[ii]=(high[di-delay-Num2][ii]-low[di-delay-Num2][ii])/f1;
			}
			     else{rank1[ii]=0;}
			}}

			QL_Oputils::rank(rank1);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c5(Num1,nan);
                float f3;
                float f4;
                float f2;

			f3 = float(high[di-delay][ii]-low[di-delay][ii]);
			f4 = float(vwap[di-delay][ii]-close[di-delay][ii]);
            for (int i= 0; i<Num1; ++i){
                c5[i] = close[di-delay-i][ii];
            }
            
            f2 = float(QL_Oputils::mean(c5));

			if ((!(GLOBALFUNC::equal(f2,0)))&&(!(GLOBALFUNC::equal(f3,0)))&&(!(GLOBALFUNC::equal(f4,0))))
			{
                //QL_Oputils::rank(vol_rank[di-delay]);
				
                rank2[ii] = float(vol_rank[di-delay][ii]) / ((f3/f2)/f4);

			}
			else {rank2[ii]=0;}
	    }
        }

        // QL_Oputils::rank(rank2);

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                alpha[ii] = float(rank1[ii]) * float(rank2[ii]);
            }
        }

        return;
		
    }
    
    
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
        }
	const QL_MATRIX<QL_FLOAT> &vwap;
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &low;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &close;
    vector<vector<float> > vol_rank;

    int Num1; 
	int Num2;

};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha83(cfg);
        return str;
    }
}

