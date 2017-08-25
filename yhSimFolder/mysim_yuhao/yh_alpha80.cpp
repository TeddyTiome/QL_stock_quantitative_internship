#include "include/qlSim.h"
namespace
{
class alpha80: public AlphaBase
{
    public:
    explicit alpha80(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        indus(dr.getData<QL_MATRIX<QL_INT>>("industry")),
        Num1(cfg->getAttributeIntDefault("para1",10)),
	Num2(cfg->getAttributeIntDefault("para2",4)),
	Num3(cfg->getAttributeIntDefault("para3",5))

    {
    	adv10.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv10[di].resize(GLOBAL::Instruments.size(),nan);
        }

    	op_ind.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            op_ind[di].resize(GLOBAL::Instruments.size(),nan);
        }

    }
    void generate(int di) override
    {

			vector<float> rank1(GLOBAL::Instruments.size(),nan);
            vector<float> rank2(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv10[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	op_ind[di-delay][ii] = open[di-delay][ii] * 0.868128 +high[di-delay][ii] *(1-0.868128); 
            }
        }
       
            QL_Oputils::industryNeutralize(op_ind[di-delay],indus[di-delay]);
            QL_Oputils::industryNeutralize(op_ind[di-delay-Num2],indus[di-delay-Num2]);
     

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {	
				rank1[ii] = op_ind[di-delay][ii] - op_ind[di-delay-Num2][ii];

                if ( !(GLOBALFUNC::equal(float(rank1[ii]),0)))
                {   
                    if (float(rank1[ii])>0){
                        rank1[ii] = 1.0;    
                    }
                    else {rank1[ii] = -1.0;}                      
                }
                else {rank1[ii] =0.0;}
            }
        }

        QL_Oputils::rank(rank1);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
		
				vector<float> c5(Num3,nan);
				vector<float> c6(Num3,nan);
				vector<float> c8(Num3,nan);

			for (int b=0; b<Num3;++b){
				for (int c=0; c<Num3; ++c){
				c6[c]= adv10[di-delay-c-b][ii];	
				c8[c]=high[di-delay-c-b][ii];
			}
				c5[b] = QL_Oputils::corr(c6,c8);
			}
			QL_Oputils::rank(c5);

			rank2[ii] = c5[0];
			}
		}

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
 				alpha[ii]= -1 * powf(float(rank1[ii]),float(rank2[ii]));	    		
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
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &open;
	const QL_MATRIX<QL_INT> &indus;
    vector<vector<float> > adv10;
    vector<vector<float> > op_ind;

        int Num1; 
	int Num2;
	int Num3;
	int Num4;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha80(cfg);
        return str;
    }
}

